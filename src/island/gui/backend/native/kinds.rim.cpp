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
#include <island/gui/backend/native/kinds.internal.hpp>
#include <island/gui/backend/native/kinds.snap.hpp>

namespace {

constexpr Float RIM = 2.0f;

auto ink(const GUI::Properties &properties) -> GFX::Color {
  if (!properties.reached || properties.glow.a == 0.0f) return properties.edge;
  return properties.glow;
}

auto band(const GUI::Properties &properties) -> Float {
  namespace NATIVE = GUI::BACKEND::NATIVE;
  if (!properties.reached)
    return NATIVE::snapped(properties.border, NATIVE::HAIRLINE);
  return NATIVE::snapped(::RIM, NATIVE::HAIRLINE);
}

}  // namespace

void GUI::KINDS::box(
  const GUI::Slot &slot, const GUI::Properties &properties,
  const GFX::Color &fill, GUI::Element *elements) {
  namespace NATIVE = BACKEND::NATIVE;
  const auto seat = NATIVE::snapped(slot);
  const auto band = ::band(properties);
  const auto out = properties.reached ? band + NATIVE::GET::pixel() : 0.0f;
  const auto edge = ::ink(properties);
  const Float x = seat.x - out, y = seat.y - out;
  const Float w = seat.w + 2 * out, h = seat.h + 2 * out;
  elements[0] = {seat.x, seat.y,    seat.w,
                 seat.h, GUI::FULL, washed(properties, fill)};
  elements[1] = {x, y, w, band, GUI::FULL, edge};
  elements[2] = {x, y + h - band, w, band, GUI::FULL, edge};
  elements[3] = {x, y + band, band, h - 2 * band, GUI::FULL, edge};
  elements[4] = {x + w - band, y + band, band, h - 2 * band, GUI::FULL, edge};
}
