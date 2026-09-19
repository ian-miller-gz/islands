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
#include <island/graphics/screen/screen.hpp>
#include <island/graphics/windows.hpp>

auto GFX::SCREEN::create(Whole width, Whole height) -> Screen {
  return {
    .target = GFX::TARGETS::create(width, height),
    .sampler = GFX::SAMPLERS::create(GFX::SAMPLERS::CLAMP),
    .width = width,
    .height = height};
}

auto GFX::SCREEN::offscreen(
  const Screen &screen, GFX::Color clear,
  Vector<GFX::Draw> draws) -> GFX::Pass {
  return {
    .clear = clear,
    .viewport =
      {0.0f, 0.0f, static_cast<Float>(screen.width),
       static_cast<Float>(screen.height)},
    .draws = std::move(draws),
    .target = screen.target};
}

auto GFX::SCREEN::present(
  const Screen &screen, GFX::Handle pipeline, GFX::Handle quad, Whole count,
  GFX::Color clear) -> GFX::Pass {
  return {
    .clear = clear,
    .viewport =
      {0.0f, 0.0f, static_cast<Float>(GFX::WINDOWS::MAIN::width),
       static_cast<Float>(GFX::WINDOWS::MAIN::height)},
    .draws = {
      {.pipeline = pipeline,
       .fragment = {.samplers = {{screen.target, screen.sampler}}},
       .vertices = quad,
       .count = count}}};
}
