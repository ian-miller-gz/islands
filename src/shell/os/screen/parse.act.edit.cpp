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
constexpr Byte INSERT = '@', DELETE = 'P', ERASE = 'X';
constexpr Byte OPEN = 'L', DROP = 'M';

auto at(SCREEN::Screen &screen, Whole offset) {
  return screen.cells.begin() + static_cast<long>(offset);
}

void characters(SCREEN::Screen &screen, Byte final, Whole count) {
  const Whole row = screen.cy * screen.cols, end = row + screen.cols;
  const Whole cursor = row + screen.cx;
  const Whole span = std::min<Whole>(count, screen.cols - screen.cx);
  if (final == ERASE)
    return (void)std::fill(
      at(screen, cursor), at(screen, cursor + span), SCREEN::Cell{});
  if (final == INSERT) {
    screen.cells.erase(at(screen, end - span), at(screen, end));
    screen.cells.insert(at(screen, cursor), span, SCREEN::Cell{});
    return;
  }
  screen.cells.erase(at(screen, cursor), at(screen, cursor + span));
  screen.cells.insert(at(screen, end - span), span, SCREEN::Cell{});
}

void rows(SCREEN::Screen &screen, Byte final, Whole count) {
  const Whole span = std::min<Whole>(count, screen.rows - screen.cy);
  const Whole row = screen.cy * screen.cols, cells = span * screen.cols;
  const Whole full = screen.rows * screen.cols;
  if (final == OPEN) {
    screen.cells.erase(at(screen, full - cells), at(screen, full));
    screen.cells.insert(at(screen, row), cells, SCREEN::Cell{});
    return;
  }
  screen.cells.erase(at(screen, row), at(screen, row + cells));
  screen.cells.insert(screen.cells.end(), cells, SCREEN::Cell{});
}
}  // namespace

auto SCREEN::edited(Screen &screen, Byte final, Whole count) -> Flag {
  if (final == INSERT || final == DELETE || final == ERASE)
    return characters(screen, final, count), true;
  if (final == OPEN || final == DROP) return rows(screen, final, count), true;
  return false;
}
