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
#include <common.hpp>
#include <string>

namespace ISLANDS::SELECT {

constexpr Whole NONE = static_cast<Whole>(-1);

inline auto cycle(Whole current, Whole count) -> Whole {
  if (count == 0) return NONE;
  return current == NONE ? 0 : (current + 1) % count;
}

template <typename Named>
auto pick(const String &which, Whole count, Named named) -> Whole {
  for (Whole index = 0; index < count; index += 1)
    if (named(index) == which || std::to_string(index) == which) return index;
  return NONE;
}

template <typename Named>
auto listing(Whole current, Whole count, Named named, const String &mark)
  -> Vector<String> {
  Vector<String> lines;
  for (Whole index = 0; index < count; index += 1)
    lines.push_back(
      std::to_string(index) + ": " + named(index) +
      (index == current ? mark : String()));
  return lines;
}

}  // namespace ISLANDS::SELECT
