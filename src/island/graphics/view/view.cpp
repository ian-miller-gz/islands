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
#include <island/graphics/view/view.internal.hpp>
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/backend/resources.hpp>
#include <island/graphics/camera.hpp>
#include <island/graphics/windows.hpp>
#include <algorithm>
#include <cmath>

GFX::VIEW::Extent GFX::VIEW::STATE::extent;
GFX::VIEW::Mode GFX::VIEW::STATE::mode = GFX::VIEW::COMPOSED;
GFX::VIEW::Fit GFX::VIEW::STATE::fit = GFX::VIEW::INTEGER;
GFX::Handle GFX::VIEW::STATE::target = GFX::NONE;
GFX::Handle GFX::VIEW::STATE::sampler = GFX::NONE;
GFX::Handle GFX::VIEW::STATE::pipeline = GFX::NONE;

namespace STATE = GFX::VIEW::STATE;

static void release() {
  if (STATE::target != GFX::NONE) GFX::TARGETS::destroy(STATE::target);
  if (STATE::sampler != GFX::NONE) GFX::SAMPLERS::destroy(STATE::sampler);
  if (STATE::pipeline != GFX::NONE) GFX::PIPELINES::destroy(STATE::pipeline);
  STATE::target = STATE::sampler = STATE::pipeline = GFX::NONE;
}

void GFX::VIEW::create(Whole width, Whole height, Mode mode, Fit fit) {
  ::release();
  STATE::extent = {width, height};
  STATE::mode = mode;
  STATE::fit = fit;
  if (mode == DIRECT) return;
  STATE::target = GFX::TARGETS::create(width, height);
  STATE::sampler = GFX::SAMPLERS::create(
    fit == LETTERBOX ? GFX::SAMPLERS::LINEAR : GFX::SAMPLERS::CLAMP);
  STATE::pipeline = GFX::PIPELINES::create(
    {.vertex = {.path = String(SHADERS) + ".vertex"},
     .fragment = {.path = String(SHADERS) + ".fragment", .samplers = 1},
     .blend = false});
}

void GFX::VIEW::reset() {
  STATE::extent = {};
  STATE::mode = COMPOSED;
  STATE::fit = INTEGER;
  STATE::target = STATE::sampler = STATE::pipeline = GFX::NONE;
}

auto GFX::VIEW::GET::extent() -> Extent { return STATE::extent; }

auto GFX::VIEW::GET::scale() -> Float {
  const auto &extent = STATE::extent;
  if (!extent.width || !extent.height) return 1.0f;
  const Float factor = std::min(
    static_cast<Float>(GFX::WINDOWS::MAIN::width) /
      static_cast<Float>(extent.width),
    static_cast<Float>(GFX::WINDOWS::MAIN::height) /
      static_cast<Float>(extent.height));
  if (STATE::fit == LETTERBOX) return factor;
  return std::max(std::floor(factor), 1.0f);
}

auto GFX::VIEW::GET::camera() -> GFX::SHADERS::Camera {
  const auto &extent = STATE::extent;
  return GFX::CAMERA::orthographic(
    0.0f, static_cast<Float>(extent.width), static_cast<Float>(extent.height),
    0.0f, NEAR, FAR);
}
