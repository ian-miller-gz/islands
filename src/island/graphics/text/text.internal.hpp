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
#include <island/graphics/text/text.hpp>
#include <island/graphics/backend/resources.hpp>

namespace GFX::TEXT {
inline const String SHADERS = COMMON::ASSETS::SHADERS + "text";
constexpr Whole LIMIT = 4096;
constexpr Whole CORNERS = 6;
constexpr Whole WIDTHS = 8;

constexpr Whole PAGE = 1024;
constexpr Whole PADDING = 1;
constexpr Whole CHANNELS = 4;

struct Shelf {
  Whole x = 0, y = 0, tall = 0;
};

struct Band {
  Whole top = 0, bottom = 0;
};

auto sizing(Float pixels) -> Font *;
auto directory(Float scale) -> Font *;
auto glyph(Font &font, Whole code) -> const Glyph *;
auto ready() -> Flag;
void reserve();
auto pack(const Vector<Byte> &coverage, Glyph &glyph) -> Flag;
void emit(
  const Glyph &glyph, Float pen, Float top, const GFX::Viewport &viewport,
  const GFX::Color &color);

namespace FACE {
inline const String PATH = COMMON::ASSETS::FONTS + "mono.ttf";

auto open() -> Status;
auto ratio(Float pixels) -> Float;
void metrics(Float ratio, Font &font);
auto covers(Whole code) -> Flag;
auto raster(Whole code, const Font &font, Glyph &glyph, Vector<Byte> &coverage)
  -> Flag;
auto kern(Whole first, Whole second, Float ratio) -> Float;
void close();
}  // namespace FACE

inline auto pair(Whole first, Whole second) -> Whole {
  return (first << 16) | second;
}
}  // namespace GFX::TEXT

namespace GFX::TEXT::STATE {
extern Map<Whole, Font> sizings;
extern Vector<Byte> pixels;
extern Shelf shelf;
extern Band owed;
extern GFX::Handle texture;
extern GFX::Handle sampler;
extern GFX::Handle pipeline;
extern GFX::Handle buffer;
extern Whole cursor;
extern Whole frame;
extern Flag ready;
extern Flag refused;
}  // namespace GFX::TEXT::STATE
