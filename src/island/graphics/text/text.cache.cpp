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

auto GFX::TEXT::sizing(Float pixels) -> Font * {
  const auto height = static_cast<Whole>(std::lround(pixels));
  if (!height) return nullptr;
  const auto found = STATE::sizings.find(height);
  if (found != STATE::sizings.end()) return &found->second;
  Font font;
  FACE::metrics(FACE::ratio(static_cast<Float>(height)), font);
  if (!font.line) return nullptr;
  font.width = font.height = static_cast<Float>(PAGE);
  return &(STATE::sizings[height] = std::move(font));
}

static auto realize(GFX::TEXT::Font &font, Whole code)
  -> const GFX::TEXT::Glyph * {
  GFX::TEXT::Glyph glyph;
  Vector<Byte> coverage;
  if (!GFX::TEXT::FACE::raster(code, font, glyph, coverage)) return nullptr;
  if (!coverage.empty() && !GFX::TEXT::pack(coverage, glyph)) return nullptr;
  return &(font.glyphs[code] = glyph);
}

auto GFX::TEXT::glyph(Font &font, Whole code) -> const Glyph * {
  if (UNICODE::control(code)) return nullptr;
  if (!FACE::covers(code)) code = UNICODE::REPLACEMENT;
  const auto found = font.glyphs.find(code);
  return found != font.glyphs.end() ? &found->second : ::realize(font, code);
}
