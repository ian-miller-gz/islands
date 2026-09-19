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
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/text/text.hpp>
#include <island/graphics/windows.hpp>
#include <island/terminal.hpp>
#include <algorithm>

void TERMINAL::fill(GFX::Viewport &viewport) {
  viewport.w = static_cast<Float>(GFX::WINDOWS::MAIN::width);
  viewport.h = static_cast<Float>(GFX::WINDOWS::MAIN::height);
}

auto TERMINAL::band(const CONSOLES::Console &console, const GFX::Pass &pass)
  -> Float {
  return pass.viewport.h / GFX::TEXT::GET::scale() * console.share;
}

auto TERMINAL::rows(
  const CONSOLES::Console &console, const GFX::Pass &pass,
  Float line) -> Whole {
  const Float room = band(console, pass) - 2 * console.margin - line;
  const auto fits = room > 0 ? static_cast<Whole>(room / line) : 0;
  return std::min<Whole>(console.lines.size(), fits);
}

auto TERMINAL::scale() -> Float { return CONSOLES::SIZE / GFX::TEXT::EM; }

auto TERMINAL::line() -> Float { return GFX::TEXT::GET::line(scale()); }

void TERMINAL::render(CONSOLES::Console &console) {
  const Float line = TERMINAL::line();
  if (!line) return;
  const Float scale = TERMINAL::scale();
  auto &pass = GFX::PASSES::last();
  if (!pass.viewport.w) fill(pass.viewport);
  const GFX::Color color = {
    console.color.r, console.color.g, console.color.b, console.color.a};
  Float top = pass.viewport.h / GFX::TEXT::GET::scale() - console.margin - line;
  GFX::TEXT::draw(
    pass, console.prompt + console.line + CONSOLES::CURSOR, console.margin, top,
    scale, PROMPT);
  console.rows = rows(console, pass, line);
  CONSOLES::page(console, 0);
  for (Whole i = 0; i < console.rows; i += 1) {
    top -= line;
    const auto row = console.lines.size() - 1 - i - console.scroll;
    GFX::TEXT::draw(
      pass, console.lines[row], console.margin, top, scale, color);
  }
}

void TERMINAL::render() {
  background();
  for (auto &console : CONSOLES::GET::consoles())
    if (console.visible) render(console);
}
