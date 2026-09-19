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
#include <island/graphics/windows.hpp>
#include <algorithm>
#include <cmath>

namespace STATE = GFX::VIEW::STATE;

static auto center(Float window, Float size) -> Float {
  const Float offset = std::floor((window - size) / 2);
  return STATE::fit == GFX::VIEW::LETTERBOX ? std::max(offset, 0.0f) : offset;
}

auto GFX::VIEW::place() -> GFX::Viewport {
  const auto scale = GET::scale();
  const Float width = static_cast<Float>(STATE::extent.width) * scale;
  const Float height = static_cast<Float>(STATE::extent.height) * scale;
  return {
    ::center(static_cast<Float>(GFX::WINDOWS::MAIN::width), width),
    ::center(static_cast<Float>(GFX::WINDOWS::MAIN::height), height), width,
    height};
}

auto GFX::VIEW::GET::viewport() -> GFX::Viewport { return place(); }

void GFX::VIEW::open(GFX::Pass &pass) {
  if (STATE::mode == COMPOSED) {
    pass.target = STATE::target;
    pass.viewport = {
      0.0f, 0.0f, static_cast<Float>(STATE::extent.width),
      static_cast<Float>(STATE::extent.height)};
    return;
  }
  pass.target = GFX::NONE;
  pass.viewport = place();
}

void GFX::VIEW::close(Vector<GFX::Pass> &passes) {
  if (STATE::mode == DIRECT) return;
  passes.push_back(
    {.viewport = place(),
     .draws = {
       {.pipeline = STATE::pipeline,
        .fragment = {.samplers = {{STATE::target, STATE::sampler}}},
        .count = POINTS}}});
}
