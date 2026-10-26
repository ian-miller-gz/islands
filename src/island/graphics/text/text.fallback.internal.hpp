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

namespace GFX::TEXT::FACE {
inline const String FALLBACK = COMMON::ASSETS::FONTS + "music.ttf";
}  // namespace GFX::TEXT::FACE

namespace GFX::TEXT::FACE::PRIMARY {
auto covers(Whole code) -> Flag;

auto raster(Whole code, const Font &font, Glyph &glyph, Vector<Byte> &coverage)
  -> Flag;
}  // namespace GFX::TEXT::FACE::PRIMARY
