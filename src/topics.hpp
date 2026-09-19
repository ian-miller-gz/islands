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
#pragma once

#include <topics/selection.hpp>

#include <common/fields.hpp>
#include <common.hpp>

namespace TOPICS {

using Message = FIELDS::Map;

constexpr Whole BACKLOG = 256;

struct Subscription {
  Whole slot;
};

using Instant = int64_t;

struct Schedule {
  Instant deadline = 0;
  Integer period = 0;
};

struct Link {
  Whole id = 0;
};

struct Broker {
  Whole id = 0;
};

#if SR_TOPICS == SR_LOCAL
auto subscribe(STRING::Hot topic) -> Subscription;

void publish(STRING::Hot topic, const Message &message);

auto drain(Subscription subscription) -> Vector<Message>;

void unsubscribe(Subscription subscription);

void reset();

auto now() -> Instant;

auto after(Integer delay) -> Schedule;

auto at(Instant instant) -> Schedule;

auto every(Integer period) -> Schedule;

void publish(STRING::Hot topic, const Message &message, Schedule when);

auto deliver(Instant instant) -> Whole;

auto link(Whole session) -> Link;

void follow(Link link, STRING::Hot topic);

void unlink(Link link);

auto serve(Whole listener) -> Broker;

void close(Broker broker);

void relay();
#else
inline auto subscribe(STRING::Hot) -> Subscription { return {}; }
inline void publish(STRING::Hot, const Message &) {}
inline auto drain(Subscription) -> Vector<Message> { return {}; }
inline void unsubscribe(Subscription) {}
inline void reset() {}
inline auto now() -> Instant { return 0; }
inline auto after(Integer) -> Schedule { return {}; }
inline auto at(Instant) -> Schedule { return {}; }
inline auto every(Integer) -> Schedule { return {}; }
inline void publish(STRING::Hot, const Message &, Schedule) {}
inline auto deliver(Instant) -> Whole { return 0; }
inline auto link(Whole) -> Link { return {}; }
inline void follow(Link, STRING::Hot) {}
inline void unlink(Link) {}
inline auto serve(Whole) -> Broker { return {}; }
inline void close(Broker) {}
inline void relay() {}
#endif

}  // namespace TOPICS

namespace TOPICS::GET {
#if SR_TOPICS == SR_LOCAL
auto dropped(Subscription subscription) -> Whole;

auto names() -> Vector<String>;

auto subscribers(STRING::Hot topic) -> Whole;

auto pending(STRING::Hot topic) -> Whole;
#else
inline auto dropped(Subscription) -> Whole { return 0; }
inline auto names() -> Vector<String> { return {}; }
inline auto subscribers(STRING::Hot) -> Whole { return 0; }
inline auto pending(STRING::Hot) -> Whole { return 0; }
#endif
}  // namespace TOPICS::GET
