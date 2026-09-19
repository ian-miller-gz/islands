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

#include <network/instances/selection.hpp>
#include <common.hpp>

namespace NETWORK::INSTANCES {

struct Entry {
  String pid;
  String name;
  String bundle;
  String socket;
};

#if SR_LISTEN == SR_UNIX
auto refresh() -> Flag;
auto list() -> const Vector<Entry> &;
auto attach(const String &pid) -> Flag;
void detach();
auto send(const String &line) -> Flag;
auto receive() -> Vector<String>;
auto arrivals() -> Whole;
auto departures() -> Whole;
void process();
void close();
#else
inline auto refresh() -> Flag { return false; }
inline auto list() -> const Vector<Entry> & {
  static const Vector<Entry> none;
  return none;
}
inline auto attach(const String &) -> Flag { return false; }
inline void detach() {}
inline auto send(const String &) -> Flag { return false; }
inline auto receive() -> Vector<String> { return {}; }
inline auto arrivals() -> Whole { return 0; }
inline auto departures() -> Whole { return 0; }
inline void process() {}
inline void close() {}
#endif
}  // namespace NETWORK::INSTANCES

namespace NETWORK::INSTANCES::GET {
#if SR_LISTEN == SR_UNIX
auto listings() -> Whole;
auto peer() -> const String &;
auto drops() -> Whole;
#else
inline auto listings() -> Whole { return 0; }
inline auto peer() -> const String & {
  static const String none;
  return none;
}
inline auto drops() -> Whole { return 0; }
#endif
}  // namespace NETWORK::INSTANCES::GET
