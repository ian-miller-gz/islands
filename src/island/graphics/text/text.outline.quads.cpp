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
#include <island/graphics/text/text.outline.internal.hpp>
#include <cstdint>
#include <cstring>

namespace STATE = GFX::TEXT::OUTLINE::STATE;
namespace OUTLINE = GFX::TEXT::OUTLINE;

constexpr Float DIAGONAL = 0.70710678f;

static auto bits(Whole low, Whole high) -> Float {
  const auto packed = static_cast<std::uint32_t>((high << 16) | low);
  Float value;
  std::memcpy(&value, &packed, sizeof value);
  return value;
}

static void corner(
  Float *slot, const OUTLINE::Glyph &glyph, Float pen, Float x, Float y,
  Float nx, Float ny, const GFX::Color &color) {
  const Float spans[] = {
    glyph.high[0] - glyph.low[0], glyph.high[1] - glyph.low[1]};
  slot[0] = pen + x;
  slot[1] = y;
  slot[2] = nx;
  slot[3] = ny;
  slot[4] = x;
  slot[5] = y;
  slot[6] = ::bits(glyph.data[0], glyph.data[1]);
  slot[7] = ::bits(glyph.bands[0] - 1, glyph.bands[1] - 1);
  slot[8] = 1;
  slot[9] = 0;
  slot[10] = 0;
  slot[11] = 1;
  slot[12] = static_cast<Float>(glyph.bands[0]) / spans[0];
  slot[13] = static_cast<Float>(glyph.bands[1]) / spans[1];
  slot[14] = -glyph.low[0] * slot[12];
  slot[15] = -glyph.low[1] * slot[13];
  slot[16] = color.r;
  slot[17] = color.g;
  slot[18] = color.b;
  slot[19] = color.a;
}

void GFX::TEXT::OUTLINE::emit(
  const Glyph &glyph, Float pen, const GFX::Color &color) {
  const Float x0 = glyph.low[0], x1 = glyph.high[0];
  const Float y0 = glyph.low[1], y1 = glyph.high[1];
  auto *slot =
    GFX::BUFFERS::GET::data<Float>(STATE::buffer) + STATE::cursor * WIDTHS;
  ::corner(slot + 0 * WIDTHS, glyph, pen, x0, y1, -DIAGONAL, DIAGONAL, color);
  ::corner(slot + 1 * WIDTHS, glyph, pen, x1, y1, DIAGONAL, DIAGONAL, color);
  ::corner(slot + 2 * WIDTHS, glyph, pen, x0, y0, -DIAGONAL, -DIAGONAL, color);
  ::corner(slot + 3 * WIDTHS, glyph, pen, x1, y1, DIAGONAL, DIAGONAL, color);
  ::corner(slot + 4 * WIDTHS, glyph, pen, x1, y0, DIAGONAL, -DIAGONAL, color);
  ::corner(slot + 5 * WIDTHS, glyph, pen, x0, y0, -DIAGONAL, -DIAGONAL, color);
  STATE::cursor += CORNERS;
}
