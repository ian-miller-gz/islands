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

#include <store.hpp>
#include <topics.hpp>

namespace TOPICS::DURABLE {

constexpr auto TABLE = "schedules";

#if SR_TOPICS == SR_LOCAL
void publish(
  STORE::Handle store, STRING::Hot schedule, STRING::Hot topic,
  const Message &message, Schedule when);

auto deliver(STORE::Handle store, Instant now) -> Whole;

void forget(STORE::Handle store, STRING::Hot schedule);
#else
inline void publish(
  STORE::Handle, STRING::Hot, STRING::Hot, const Message &, Schedule) {}
inline auto deliver(STORE::Handle, Instant) -> Whole { return 0; }
inline void forget(STORE::Handle, STRING::Hot) {}
#endif

}  // namespace TOPICS::DURABLE

namespace TOPICS::DURABLE::GET {
#if SR_TOPICS == SR_LOCAL
auto schedules(STORE::Handle store) -> Vector<String>;

auto due(STORE::Handle store, STRING::Hot schedule) -> Instant;
#else
inline auto schedules(STORE::Handle) -> Vector<String> { return {}; }
inline auto due(STORE::Handle, STRING::Hot) -> Instant { return 0; }
#endif
}  // namespace TOPICS::DURABLE::GET
