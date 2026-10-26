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
#include <island/gui/events.internal.hpp>
#include <cmath>

namespace {

constexpr Float TRAVEL = 200.0f;

}  // namespace

auto GUI::spin(Handle document, Document &entry, Whole index, Float x, Float y)
  -> Flag {
  auto &pointer = entry.pointer;
  const auto &properties = entry.nodes[index].properties;
  if (properties.graphic != Dial::DIAL || properties.most <= properties.least)
    return false;
  const Float across = x - pointer.grab.x;
  const Float down = y - pointer.grab.y;
  const Float pull = std::abs(across) >= std::abs(down) ? across : -down;
  if (!pointer.dragged && std::abs(pull) < GUI::THRESHOLD) return true;
  if (!pointer.dragged) pointer.armed = PARENTLESS;
  pointer.dragged = true;
  const Float span = properties.most - properties.least;
  Float moved = pull / ::TRAVEL * span;
  if (pointer.chord.shift) moved /= FINE;
  if (properties.steps > 0) {
    const Float step = span / static_cast<Float>(properties.steps);
    moved = std::trunc(moved / step) * step;
  }
  const Float value = pointer.grabbed + moved;
  turn(document, entry, index, quantize(properties, pointer.chord, value));
  return true;
}
