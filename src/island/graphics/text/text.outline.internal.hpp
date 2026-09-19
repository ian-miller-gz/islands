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
#include <island/graphics/text/text.internal.hpp>

namespace GFX::TEXT::OUTLINE {
inline const String SLIM = COMMON::ASSETS::FONTS + "mono";
inline const String WIDE = COMMON::ASSETS::FONTS + "mono.full";
constexpr STRING::Hot SUFFIX = ".outline";
inline const String SHADERS = COMMON::ASSETS::SHADERS + "slug";
constexpr Whole LIMIT = 1024;
constexpr Whole CORNERS = 6;
constexpr Whole WIDTHS = 20;

struct Glyph {
  Whole data[2] = {0, 0};
  Whole bands[2] = {0, 0};
  Float low[2] = {0, 0};
  Float high[2] = {0, 0};
  Float advance = 0;
};

struct Face {
  Float line = 0, base = 0;
  Whole curves[2] = {0, 0};
  Whole strips[2] = {0, 0};
  String sheets[2];
  Map<Whole, Glyph> glyphs;
  Map<Whole, Float> kernings;
};

struct Camera {
  Float matrix[16];
  Float viewport[4] = {0, 0, 0, 0};
};

auto tier() -> String;
auto parse(const String &path, Face &face) -> Status;
auto ready() -> Flag;
auto upload() -> Status;
void emit(const Glyph &glyph, Float pen, const GFX::Color &color);
void close();
}  // namespace GFX::TEXT::OUTLINE

namespace GFX::TEXT::OUTLINE::STATE {
extern Face face;
extern GFX::Handle curves;
extern GFX::Handle strips;
extern GFX::Handle sampler;
extern GFX::Handle pipeline;
extern GFX::Handle buffer;
extern Whole cursor;
extern Whole frame;
extern Flag ready;
extern Flag refused;
}  // namespace GFX::TEXT::OUTLINE::STATE
