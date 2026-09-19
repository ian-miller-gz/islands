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

namespace SCREEN = SHELL::OS::SCREEN;

namespace {
constexpr Float RULE = 0.08f;
constexpr Float SEAT = 0.92f;

auto backing(
  const Vector<SCREEN::Cell> &cells, const SCREEN::Metrics &m, Whole row,
  Whole col) -> Whole {
  const auto &bg = cells[row * m.cols + col].bg;
  Whole end = col;
  while (end < m.cols && SCREEN::same(cells[row * m.cols + end].bg, bg))
    end += 1;
  return end;
}

auto ruled(
  const Vector<SCREEN::Cell> &cells, const SCREEN::Metrics &m, Whole row,
  Whole col) -> Whole {
  const Flag mark = cells[row * m.cols + col].underline;
  Whole end = col;
  while (end < m.cols && cells[row * m.cols + end].underline == mark) end += 1;
  return end;
}
}  // namespace

void SCREEN::cells(const Metrics &m) {
  const auto &cells = GET::grid().cells;
  for (Whole row = 0; row < m.rows; row += 1) {
    for (Whole col = 0; col < m.cols;) {
      const Whole end = backing(cells, m, row, col);
      const auto &bg = cells[row * m.cols + col].bg;
      if (!same(bg, BACKGROUND))
        quad(
          m.x + col * m.cell, m.y + row * m.line, (end - col) * m.cell, m.line,
          bg);
      col = end;
    }
    for (Whole col = 0; col < m.cols;) {
      const Whole end = ruled(cells, m, row, col);
      if (cells[row * m.cols + col].underline)
        quad(
          m.x + col * m.cell, m.y + (row + SEAT) * m.line, (end - col) * m.cell,
          m.line * RULE, cells[row * m.cols + col].fg);
      col = end;
    }
  }
}
