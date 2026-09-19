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
#include <island/graphics/text/text.internal.hpp>
#include <cmath>

namespace STATE = GFX::TEXT::STATE;

static void emit(
  Float *slot, Float x, Float y, Float u, Float v, const GFX::Color &color) {
  slot[0] = x;
  slot[1] = y;
  slot[2] = u;
  slot[3] = v;
  slot[4] = color.r;
  slot[5] = color.g;
  slot[6] = color.b;
  slot[7] = color.a;
}

void GFX::TEXT::emit(
  const Glyph &glyph, Float pen, Float top, const GFX::Viewport &viewport,
  const GFX::Color &color) {
  const Float left = std::round(pen + glyph.bearing[0]);
  const Float down = std::round(top + glyph.bearing[1]);
  const Float x = 2.0f * left / viewport.w - 1.0f;
  const Float y = 1.0f - 2.0f * down / viewport.h;
  const Float w = 2.0f * glyph.width / viewport.w;
  const Float h = 2.0f * glyph.height / viewport.h;
  const Float page = static_cast<Float>(GFX::TEXT::PAGE);
  const Float u = glyph.x / page;
  const Float v = glyph.y / page;
  const Float s = u + glyph.width / page;
  const Float t = v + glyph.height / page;
  auto *slot = GFX::BUFFERS::GET::data<Float>(STATE::buffer) +
               STATE::cursor * GFX::TEXT::WIDTHS;
  ::emit(slot + 0 * GFX::TEXT::WIDTHS, x, y, u, v, color);
  ::emit(slot + 1 * GFX::TEXT::WIDTHS, x + w, y, s, v, color);
  ::emit(slot + 2 * GFX::TEXT::WIDTHS, x, y - h, u, t, color);
  ::emit(slot + 3 * GFX::TEXT::WIDTHS, x + w, y, s, v, color);
  ::emit(slot + 4 * GFX::TEXT::WIDTHS, x + w, y - h, s, t, color);
  ::emit(slot + 5 * GFX::TEXT::WIDTHS, x, y - h, u, t, color);
  STATE::cursor += GFX::TEXT::CORNERS;
}
