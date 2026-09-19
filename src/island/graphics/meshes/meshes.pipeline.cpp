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
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/meshes/meshes.pipeline.hpp>
#include <island/graphics/shaders/blocks.hpp>

namespace {
const String PAIR = COMMON::ASSETS::SHADERS + "flat";
}

auto GFX::MESHES::pipeline() -> GFX::Handle {
  return GFX::PIPELINES::create(
    {.vertex =
       {.path = PAIR + ".vertex", .uniforms = {sizeof(GFX::SHADERS::Camera)}},
     .fragment = {.path = PAIR + ".fragment"},
     .attributes = {2, 4},
     .blend = false});
}
