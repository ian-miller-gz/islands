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
#include <chrono>

#include <topics.hpp>
#include <topics/local/local.internal.hpp>

namespace {

struct Pending {
  String topic;
  TOPICS::Message message;
  TOPICS::Instant deadline;
  Integer period;
};

Vector<Pending> pending;

}  // namespace

namespace TOPICS {

auto now() -> Instant {
  const auto since = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::seconds>(since).count();
}

auto after(Integer delay) -> Schedule { return {now() + delay, 0}; }

auto at(Instant instant) -> Schedule { return {instant, 0}; }

auto every(Integer period) -> Schedule { return {now() + period, period}; }

void publish(STRING::Hot topic, const Message &message, Schedule when) {
  pending.push_back(Pending{topic, message, when.deadline, when.period});
}

auto deliver(Instant now) -> Whole {
  Vector<Whole> due;
  for (Whole index = 0; index < pending.size(); ++index)
    if (pending[index].deadline <= now) due.push_back(index);
  std::stable_sort(due.begin(), due.end(), [](Whole left, Whole right) {
    return pending[left].deadline < pending[right].deadline;
  });
  for (Whole index : due) {
    Pending &entry = pending[index];
    publish(entry.topic.c_str(), entry.message);
    if (entry.period > 0)
      entry.deadline +=
        entry.period * ((now - entry.deadline) / entry.period + 1);
  }
  std::erase_if(pending, [now](const Pending &entry) {
    return entry.period <= 0 && entry.deadline <= now;
  });
  return static_cast<Whole>(due.size());
}

}  // namespace TOPICS

void TOPICS::SCHEDULE::purge() { pending.clear(); }
