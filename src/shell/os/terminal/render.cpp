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
#include <shell/os/terminal/render.internal.hpp>
#include <shell/os/screen.hpp>
#include <shell/os/pty.hpp>
#include <island/graphics/sprites/sprites.hpp>
#include <island/graphics/backend/resources.hpp>
#include <island/graphics/camera.hpp>
#include <console.hpp>

namespace SCREEN = SHELL::OS::SCREEN;

namespace {
constexpr Whole QUADS =
  2 * SHELL::OS::PTY::WIDEST * SHELL::OS::PTY::TALLEST + 2;
constexpr Float BAR = 0.15f;

GFX::SPRITES::Handle atlas = GFX::SPRITES::NONE;
GFX::SPRITES::Handle batch = GFX::SPRITES::NONE;

void realize() {
  if (batch != GFX::SPRITES::NONE) return;
  constexpr Byte LIT = static_cast<Byte>(0xFF);
  atlas = GFX::SPRITES::ATLASES::create(
    1, 1, {}, Vector<Byte>(GFX::TEXTURES::STRIDE, LIT));
  batch = GFX::SPRITES::create(atlas, QUADS);
}
}  // namespace

void SCREEN::quad(Float x, Float y, Float w, Float h, const GFX::Color &color) {
  GFX::SPRITES::push(batch, {.x = x, .y = y, .w = w, .h = h, .color = color});
}

void SCREEN::draw(GFX::Pass &pass) {
  if (!GFX::TEXT::GET::line(CONSOLES::SIZE / GFX::TEXT::EM)) return;
  realize();
  const auto m = place(pass);
  const Float scale = GFX::TEXT::GET::scale();
  GFX::SPRITES::write(
    batch, GFX::CAMERA::orthographic(
             0.0f, pass.viewport.w / scale, pass.viewport.h / scale, 0.0f));
  quad(0.0f, 0.0f, pass.viewport.w / scale, m.band, BACKGROUND);
  cells(m);
  const auto &grid = GET::grid();
  if (grid.cursor)
    quad(
      m.x + grid.cx * m.cell, m.y + (grid.cy + 1 - BAR) * m.line, m.cell,
      m.line * BAR, FOREGROUND);
  GFX::SPRITES::flush(batch, pass);
  glyphs(pass, m);
}

void SHELL::OS::SCREEN::drop() {
  batch = GFX::SPRITES::NONE;
  atlas = GFX::SPRITES::NONE;
}
