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
// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/graphics/model/model.pipeline.hpp>
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/shaders/blocks.hpp>

namespace {
const String PAIR = COMMON::ASSETS::SHADERS + "model";

auto description() -> GFX::Pipeline {
  return {
    .vertex =
      {.path = PAIR + ".vertex", .uniforms = {sizeof(GFX::SHADERS::Camera)}},
    .fragment =
      {.path = PAIR + ".fragment",
       .samplers = 1,
       .uniforms = {sizeof(GFX::Color)}},
    .attributes = GFX::MODEL::LAYOUT::ATTRIBUTES,
    .blend = false,
    .depth = true};
}
}  // namespace

auto GFX::MODEL::pipeline() -> GFX::Handle {
  return GFX::PIPELINES::create(description());
}

auto GFX::MODEL::family() -> GFX::Handle {
  return GFX::PIPELINES::FAMILIES::create(description());
}
