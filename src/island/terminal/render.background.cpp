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
#include <island/terminal/render.internal.hpp>
#include <island/graphics/sprites/sprites.hpp>
#include <island/graphics/backend/resources.hpp>
#include <island/graphics/text/text.hpp>
#include <island/graphics/camera.hpp>
#include <island/terminal.hpp>

namespace {
constexpr Whole PANELS = 16;

GFX::SPRITES::Handle atlas = GFX::SPRITES::NONE;
GFX::SPRITES::Handle batch = GFX::SPRITES::NONE;

void realize() {
  if (batch != GFX::SPRITES::NONE) return;
  constexpr Byte LIT = static_cast<Byte>(0xFF);
  atlas = GFX::SPRITES::ATLASES::create(
    1, 1, {}, Vector<Byte>(GFX::TEXTURES::STRIDE, LIT));
  batch = GFX::SPRITES::create(atlas, PANELS);
}

GFX::SPRITES::Sprite panel(
  const CONSOLES::Console &console, const GFX::Pass &pass) {
  const Float scale = GFX::TEXT::GET::scale();
  const Float band = TERMINAL::band(console, pass);
  return {
    .x = 0.0f,
    .y = pass.viewport.h / scale - band,
    .w = pass.viewport.w / scale,
    .h = band,
    .color = TERMINAL::BACKGROUND};
}
}  // namespace

void TERMINAL::background() {
  const Float line = TERMINAL::line();
  if (!line) return;
  realize();
  auto &pass = GFX::PASSES::last();
  if (!pass.viewport.w) fill(pass.viewport);
  const Float scale = GFX::TEXT::GET::scale();
  const auto camera = GFX::CAMERA::orthographic(
    0.0f, pass.viewport.w / scale, pass.viewport.h / scale, 0.0f);
  GFX::SPRITES::write(batch, camera);
  for (auto &console : CONSOLES::GET::consoles())
    if (console.visible) GFX::SPRITES::push(batch, panel(console, pass));
  GFX::SPRITES::flush(batch, pass);
}

void TERMINAL::reset() {
  batch = GFX::SPRITES::NONE;
  atlas = GFX::SPRITES::NONE;
}
