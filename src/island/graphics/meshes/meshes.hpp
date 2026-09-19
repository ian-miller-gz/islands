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

#include <island/graphics/color.hpp>
#include <island/graphics/model/model.hpp>
#include <common.hpp>

namespace GFX::MESHES {

constexpr Whole VERTICES = 6;
constexpr Whole COLORED = 2 + 4;
constexpr Whole TEXTURED = 2 + 2;

auto quad(Float x, Float y, Float w, Float h, GFX::Color color)
  -> Vector<Float>;

auto quad(Float x, Float y, Float w, Float h) -> Vector<Float>;

auto fullscreen() -> Vector<Float>;

auto cube() -> GFX::MODEL::Scene;

auto plane() -> GFX::MODEL::Scene;

auto grid(Float extent, Whole steps) -> Vector<Float>;

}  // namespace GFX::MESHES
