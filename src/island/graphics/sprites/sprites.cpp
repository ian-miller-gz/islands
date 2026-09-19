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
#include <island/graphics/sprites/sprites.internal.hpp>
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/backend/resources.hpp>
#include <island/graphics/camera.hpp>
#include <island/graphics/windows.hpp>

Vector<GFX::SPRITES::Batch> GFX::SPRITES::STATE::batches;
Vector<GFX::SPRITES::Atlas> GFX::SPRITES::STATE::atlases;
GFX::Handle GFX::SPRITES::STATE::sampler = GFX::NONE;
GFX::SPRITES::Handle GFX::SPRITES::STATE::white = GFX::SPRITES::NONE;

namespace STATE = GFX::SPRITES::STATE;

auto GFX::SPRITES::create(Handle atlas, Whole limit, Flag snap) -> Handle {
  const auto pipeline = GFX::PIPELINES::create(
    {.vertex =
       {.path = String(SHADERS) + ".vertex",
        .uniforms = {sizeof(GFX::SHADERS::Camera)},
        .buffers = 1},
     .fragment = {.path = String(SHADERS) + ".fragment", .samplers = 1}});
  const auto buffer = GFX::BUFFERS::create(limit * sizeof(Sprite));
  STATE::batches.push_back({buffer, pipeline, atlas, limit, 0, snap});
  return static_cast<Handle>(STATE::batches.size() - 1);
}

void GFX::SPRITES::write(Handle batch, const GFX::SHADERS::Camera &camera) {
  GFX::PIPELINES::write(STATE::batches[batch].pipeline, CAMERA, &camera);
}

auto GFX::SPRITES::screen() -> GFX::SHADERS::Camera {
  return GFX::CAMERA::orthographic(
    0.0f, static_cast<Float>(GFX::WINDOWS::MAIN::width),
    static_cast<Float>(GFX::WINDOWS::MAIN::height), 0.0f);
}

void GFX::SPRITES::reset() {
  STATE::batches.clear();
  STATE::atlases.clear();
  STATE::sampler = GFX::NONE;
  STATE::white = NONE;
}

auto GFX::SPRITES::GET::limit(Handle batch) -> Whole {
  return STATE::batches[batch].limit;
}
