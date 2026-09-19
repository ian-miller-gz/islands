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
#include <console/console.internal.hpp>
#include <algorithm>

void CONSOLES::append(Console &console, const String &line) {
  console.lines.push_back(line);
  if (console.lines.size() > LIMIT) console.lines.erase(console.lines.begin());
  if (console.scroll) {
    console.scroll += 1;
    page(console, 0);
  }
}

void CONSOLES::page(Console &console, Integer pages) {
  const auto rows = console.rows ? console.rows : 1;
  const Integer step = pages * static_cast<Integer>(rows);
  const auto held = console.lines.size();
  const Integer ceiling = held > rows ? static_cast<Integer>(held - rows) : 0;
  const Integer moved = static_cast<Integer>(console.scroll) + step;
  console.scroll = static_cast<Whole>(std::clamp<Integer>(moved, 0, ceiling));
}
