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
#include <shell/os/screen/screen.internal.hpp>

namespace SCREEN = SHELL::OS::SCREEN;

namespace {
constexpr Whole INDEXED = 5, DIRECT = 2;
constexpr Whole CUBE = 16, RAMP = 232;
constexpr Whole SIDE = 6;
constexpr Float FULL = 255.0f;
constexpr Whole STEP = 40, FLOOR = 55;
constexpr Whole GREY = 10, DARKEST = 8;

auto level(Whole step) -> Float {
  return step ? (FLOOR + step * STEP) / FULL : 0.0f;
}
}  // namespace

auto SCREEN::indexed(Whole index) -> GFX::Color {
  if (index < CUBE) return PALETTE[index];
  if (index >= RAMP) {
    const Float grey = (DARKEST + (index - RAMP) * GREY) / FULL;
    return {grey, grey, grey, 1.0f};
  }
  const Whole slot = index - CUBE;
  return {
    level(slot / (SIDE * SIDE)), level(slot / SIDE % SIDE), level(slot % SIDE),
    1.0f};
}

auto SCREEN::extended(
  const Vector<Whole> &values, Whole &index, GFX::Color &color) -> Flag {
  const auto after = values.size() - index - 1;
  const Whole form = after ? values[index + 1] : 0;
  if (form == INDEXED && after >= 2) {
    color = indexed(values[index + 2]);
    index += 2;
    return true;
  }
  if (form == DIRECT && after >= 4) {
    color = {
      values[index + 2] / FULL, values[index + 3] / FULL,
      values[index + 4] / FULL, 1.0f};
    index += 4;
    return true;
  }
  return false;
}
