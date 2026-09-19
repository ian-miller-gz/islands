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
#include <shell/os/pty.hpp>
#include <console.hpp>
#include <algorithm>

namespace SCREEN = SHELL::OS::SCREEN;

auto SCREEN::place(const GFX::Pass &pass) -> Metrics {
  const Float display = GFX::TEXT::GET::scale();
  const Float scale = CONSOLES::SIZE / GFX::TEXT::EM;
  const Float cell = GFX::TEXT::GET::advance(scale);
  const Float line = GFX::TEXT::GET::line(scale);
  if (cell <= 0 || line <= 0) return {};
  const Float band = pass.viewport.h / display * CONSOLES::SHARE;
  const Float room = pass.viewport.w / display - 2 * MARGIN;
  const auto cols = std::clamp<Whole>(
    static_cast<Whole>(std::max(room, 0.0f) / cell), 1, SHELL::OS::PTY::WIDEST);
  const auto rows = std::clamp<Whole>(
    static_cast<Whole>(std::max(band - 2 * MARGIN, 0.0f) / line), 1,
    SHELL::OS::PTY::TALLEST);
  fit(cols, rows);
  return {MARGIN, MARGIN, cell, line, cols, rows, scale, band};
}

void SCREEN::glyphs(GFX::Pass &pass, const Metrics &m) {
  const auto &cells = GET::grid().cells;
  for (Whole row = 0; row < m.rows; row += 1)
    for (Whole col = 0; col < m.cols;) {
      const auto &fg = cells[row * m.cols + col].fg;
      String run;
      Whole end = col;
      for (; end < m.cols && same(cells[row * m.cols + end].fg, fg); end += 1)
        run.push_back(static_cast<char>(cells[row * m.cols + end].glyph));
      GFX::TEXT::draw(
        pass, run, m.x + col * m.cell, m.y + row * m.line, m.scale, fg);
      col = end;
    }
}
