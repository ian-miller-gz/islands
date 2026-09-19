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

#include <island/graphics/backend/passes.hpp>
#include <common.hpp>

namespace GFX::TEXT {

constexpr Float EM = 32;

constexpr Whole REFERENCE = 'M';

struct Glyph {
  Float x = 0, y = 0;
  Float width = 0, height = 0;
  Float bearing[2] = {0, 0};
  Float advance = 0;
};

struct Font {
  Float ratio = 0;
  Float line = 0;
  Float ascent = 0;
  Float advance = 0;
  Float width = 0, height = 0;
  Map<Whole, Glyph> glyphs;
  Map<Whole, Float> kernings;
};

auto draw(
  GFX::Pass &pass, const String &text, Float x, Float y, Float scale,
  const GFX::Color &color) -> Whole;

void draw(
  GFX::Pass &pass, const String &text, const Float matrix[16],
  const GFX::Color &color);

void flush();

void close();
}  // namespace GFX::TEXT

namespace GFX::TEXT::GET {

auto line(Float scale = 1) -> Float;

auto scale() -> Float;

auto advance(Float scale = 1) -> Float;

auto font(Float scale = 1) -> const Font &;

auto glyph(Whole code, Float scale = 1) -> const Glyph *;

auto sampling() -> GFX::Sampling;

}  // namespace GFX::TEXT::GET
