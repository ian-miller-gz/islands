#pragma region COPYRIGHT
/* ===========================================================================
 *              (C) Copyright 2025 - Islands-Engine - Ian Miller             *
 =============================================================================
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU Affero General Public License as           *
 *  published by the Free Software Foundation, either version 3 of the       *
 *  License, or (at your option) any later version.                          *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Affero General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU Affero General Public License *
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 ============================================================================*/
#pragma endregion
#include <algorithm>
#include <charconv>
#include <string>
#include <string_view>

#include <topics/durable.hpp>

namespace {

using TOPICS::Instant;

constexpr auto TOPIC = "topic";
constexpr auto DEADLINE = "deadline";
constexpr auto PERIOD = "period";
constexpr std::string_view FIELD = "field.";

struct Filed {
  String name;
  String topic;
  TOPICS::Message message;
  Instant deadline = 0;
  Integer period = 0;
};

auto stamp(Instant instant) -> String { return std::to_string(instant); }

auto instant(const FIELDS::Value &value) -> Instant {
  const auto *text = std::get_if<String>(&value);
  Instant parsed = 0;
  if (text) std::from_chars(text->data(), text->data() + text->size(), parsed);
  return parsed;
}

auto record(const Filed &entry) -> FIELDS::Map {
  FIELDS::Map fields{
    {TOPIC, entry.topic},
    {DEADLINE, stamp(entry.deadline)},
    {PERIOD, entry.period}};
  for (const auto &[name, value] : entry.message)
    fields[String(FIELD) + name] = value;
  return fields;
}

auto filed(const STORE::Row &row) -> Filed {
  Filed entry;
  entry.name = row.key;
  for (const auto &[name, value] : row.fields) {
    if (name == TOPIC)
      entry.topic = std::get<String>(value);
    else if (name == DEADLINE)
      entry.deadline = instant(value);
    else if (name == PERIOD)
      entry.period = std::get<Integer>(value);
    else if (name.starts_with(FIELD))
      entry.message[name.substr(FIELD.size())] = value;
  }
  return entry;
}

}  // namespace

namespace TOPICS::DURABLE {

void publish(
  STORE::Handle store, STRING::Hot schedule, STRING::Hot topic,
  const Message &message, Schedule when) {
  STORE::put(
    store, TABLE, schedule,
    record({schedule, topic, message, when.deadline, when.period}));
}

auto deliver(STORE::Handle store, Instant now) -> Whole {
  Vector<Filed> due;
  for (const auto &row : STORE::scan(store, TABLE)) {
    auto entry = filed(row);
    if (entry.deadline <= now) due.push_back(entry);
  }
  std::stable_sort(
    due.begin(), due.end(), [](const Filed &left, const Filed &right) {
      return left.deadline < right.deadline;
    });
  for (auto &entry : due) {
    TOPICS::publish(entry.topic.c_str(), entry.message);
    if (entry.period <= 0) {
      STORE::erase(store, TABLE, entry.name.c_str());
      continue;
    }
    entry.deadline +=
      entry.period * ((now - entry.deadline) / entry.period + 1);
    STORE::put(store, TABLE, entry.name.c_str(), record(entry));
  }
  return static_cast<Whole>(due.size());
}

void forget(STORE::Handle store, STRING::Hot schedule) {
  STORE::erase(store, TABLE, schedule);
}

}  // namespace TOPICS::DURABLE

namespace TOPICS::DURABLE::GET {

auto schedules(STORE::Handle store) -> Vector<String> {
  Vector<String> names;
  for (const auto &row : STORE::scan(store, TABLE)) names.push_back(row.key);
  std::sort(names.begin(), names.end());
  return names;
}

auto due(STORE::Handle store, STRING::Hot schedule) -> Instant {
  FIELDS::Map fields;
  if (!STORE::get(store, TABLE, schedule, fields)) return 0;
  const auto found = fields.find(DEADLINE);
  return found == fields.end() ? 0 : instant(found->second);
}

}  // namespace TOPICS::DURABLE::GET
