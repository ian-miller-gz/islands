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
#include <island/gui/nga.internal.hpp>
#include <algorithm>

auto GUI::NGA::crosses(const Span &span, Position from, Position to) -> Flag {
  const Float travel[2] = {to.x - from.x, to.y - from.y};
  const Float least[2] = {span.x, span.y};
  const Float most[2] = {span.x + span.w, span.y + span.h};
  const Float start[2] = {from.x, from.y};
  Float near = 0.0f;
  Float far = 1.0f;
  for (Whole axis = 0; axis < 2; axis += 1) {
    if (travel[axis] == 0.0f) {
      if (start[axis] < least[axis] || start[axis] > most[axis]) return false;
      continue;
    }
    Float first = (least[axis] - start[axis]) / travel[axis];
    Float last = (most[axis] - start[axis]) / travel[axis];
    if (first > last) std::swap(first, last);
    near = std::max(near, first);
    far = std::min(far, last);
    if (near > far) return false;
  }
  return true;
}
