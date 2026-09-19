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
#include <shell/os/screen/screen.internal.hpp>
#include <algorithm>

namespace SCREEN = SHELL::OS::SCREEN;

namespace {
auto carried(
  const Vector<SCREEN::Cell> &cells, Whole was, Whole wide, Whole skip,
  Whole cols, Whole rows) -> Vector<SCREEN::Cell> {
  Vector<SCREEN::Cell> fresh(cols * rows);
  if (!was || !wide) return fresh;
  const Whole deep = std::min<Whole>(rows, was - skip);
  const Whole span = std::min<Whole>(cols, wide);
  for (Whole row = 0; row < deep; row += 1)
    for (Whole col = 0; col < span; col += 1)
      fresh[row * cols + col] = cells[(skip + row) * wide + col];
  return fresh;
}
}  // namespace

void SCREEN::resize(Screen &screen, Whole cols, Whole rows) {
  if (!cols || !rows) return;
  if (screen.cols == cols && screen.rows == rows) return;
  if (!screen.cols || !screen.rows) return wipe(screen, cols, rows);
  const Whole skip = screen.rows > rows ? screen.rows - rows : 0;
  screen.cells =
    carried(screen.cells, screen.rows, screen.cols, skip, cols, rows);
  if (!screen.held.empty())
    screen.held =
      carried(screen.held, screen.rows, screen.cols, skip, cols, rows);
  screen.cy = std::min<Whole>(rows - 1, screen.cy - std::min(screen.cy, skip));
  screen.cx = std::min<Whole>(cols - 1, screen.cx);
  screen.hy = std::min<Whole>(rows - 1, screen.hy - std::min(screen.hy, skip));
  screen.hx = std::min<Whole>(cols - 1, screen.hx);
  screen.cols = cols;
  screen.rows = rows;
}
