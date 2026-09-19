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
#include <utility>

#include <topics.hpp>
#include <topics/local/local.internal.hpp>

namespace {

struct Slot {
  String topic;
  Vector<TOPICS::Message> backlog;
  Whole dropped = 0;
  Flag live = false;
};

Vector<Slot> slots;

auto find(TOPICS::Subscription handle) -> Slot * {
  if (handle.slot == 0 || handle.slot > slots.size()) return nullptr;
  Slot &slot = slots[handle.slot - 1];
  return slot.live ? &slot : nullptr;
}

}  // namespace

namespace TOPICS {

auto subscribe(STRING::Hot topic) -> Subscription {
  for (Whole index = 0; index < slots.size(); ++index) {
    if (slots[index].live) continue;
    slots[index] = Slot{topic, {}, 0, true};
    return {index + 1};
  }
  slots.push_back(Slot{topic, {}, 0, true});
  return {static_cast<Whole>(slots.size())};
}

void publish(STRING::Hot topic, const Message &message) {
  for (Slot &slot : slots) {
    if (!slot.live || slot.topic != topic) continue;
    if (slot.backlog.size() < TOPICS::BACKLOG)
      slot.backlog.push_back(message);
    else
      ++slot.dropped;
  }
}

auto drain(Subscription subscription) -> Vector<Message> {
  Slot *slot = find(subscription);
  if (!slot) return {};
  return std::exchange(slot->backlog, {});
}

void unsubscribe(Subscription subscription) {
  if (Slot *slot = find(subscription)) *slot = Slot{};
}

void reset() {
  LINK::purge();
  BROKER::purge();
  slots.clear();
  SCHEDULE::purge();
}

}  // namespace TOPICS

auto TOPICS::GET::dropped(Subscription subscription) -> Whole {
  const Slot *slot = find(subscription);
  return slot ? slot->dropped : 0;
}

auto TOPICS::GET::names() -> Vector<String> {
  Vector<String> names;
  for (const Slot &slot : slots) {
    if (!slot.live) continue;
    if (std::find(names.begin(), names.end(), slot.topic) == names.end())
      names.push_back(slot.topic);
  }
  std::sort(names.begin(), names.end());
  return names;
}

auto TOPICS::GET::subscribers(STRING::Hot topic) -> Whole {
  Whole count = 0;
  for (const Slot &slot : slots)
    if (slot.live && slot.topic == topic) ++count;
  return count;
}

auto TOPICS::GET::pending(STRING::Hot topic) -> Whole {
  Whole total = 0;
  for (const Slot &slot : slots)
    if (slot.live && slot.topic == topic) total += slot.backlog.size();
  return total;
}
