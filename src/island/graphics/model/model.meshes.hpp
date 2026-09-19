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

#include <island/graphics/backend/resources.hpp>
#include <island/graphics/color.hpp>
#include <common.hpp>

namespace GFX::MODEL {

struct Material {
  GFX::Color color = GFX::COLORS::WHITE;
  GFX::Handle texture = GFX::NONE;
  Float metallic = 1.0f;
  Float roughness = 1.0f;
};

struct Mesh {
  GFX::Handle vertices = GFX::NONE;
  GFX::Handle indices = GFX::NONE;
  Whole count = 0;
  Material material;
};

}  // namespace GFX::MODEL
