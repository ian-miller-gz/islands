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
#include <island/gui/backend/native/kinds.snap.hpp>
#include <island/graphics/text/text.hpp>
#include <algorithm>
#include <cmath>

namespace {

constexpr Float UNSCALED = 1.0f;

auto scale() -> Float {
  const Float held = GFX::TEXT::GET::scale();
  return held > 0.0f ? held : UNSCALED;
}

}  // namespace

auto GUI::BACKEND::NATIVE::snapped(Float edge) -> Float {
  const Float device = ::scale();
  return std::round(edge * device) / device;
}

auto GUI::BACKEND::NATIVE::snapped(Float weight, Float floor) -> Float {
  if (weight <= 0.0f) return 0.0f;
  const Float device = ::scale();
  return std::max(std::round(weight * device), floor) / device;
}

auto GUI::BACKEND::NATIVE::snapped(const Slot &slot) -> Slot {
  Slot seat = slot;
  seat.x = snapped(slot.x);
  seat.y = snapped(slot.y);
  seat.w = snapped(slot.x + slot.w) - seat.x;
  seat.h = snapped(slot.y + slot.h) - seat.y;
  return seat;
}

auto GUI::BACKEND::NATIVE::GET::pixel() -> Float {
  return UNSCALED / ::scale();
}
