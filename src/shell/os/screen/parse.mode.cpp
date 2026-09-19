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
#include <utility>

namespace SCREEN = SHELL::OS::SCREEN;

namespace {
constexpr Byte PRIVATE = '?', SECONDARY = '>';
constexpr Byte SET = 'h', RESET = 'l', STATUS = 'n', ATTRIBUTES = 'c';
constexpr Whole ARROWS = 1;
constexpr Whole CURSOR = 25;
constexpr Whole SCREENS = 47;
constexpr Whole ALTERNATE = 1049;
constexpr Whole HEALTH = 5, POSITION = 6;
constexpr STRING::Hot READY = "\x1b[0n";
constexpr STRING::Hot IDENTITY = "\x1b[?1;2c";

void alternate(SCREEN::Screen &screen, Flag front) {
  if (front == screen.alternate) return;
  screen.alternate = front;
  std::swap(screen.cells, screen.held);
  std::swap(screen.cx, screen.hx);
  std::swap(screen.cy, screen.hy);
  if (!front) return;
  screen.cells.assign(screen.cols * screen.rows, SCREEN::Cell{});
  screen.cx = screen.cy = 0;
}

void dec(SCREEN::Screen &screen, Whole code, Flag on) {
  if (code == ARROWS) screen.cursors = on;
  if (code == CURSOR) screen.cursor = on;
  if (code == SCREENS || code == ALTERNATE) alternate(screen, on);
}

void status(SCREEN::Screen &screen, Whole code) {
  if (code == HEALTH) screen.replies += READY;
  if (code == POSITION)
    screen.replies += "\x1b[" + std::to_string(screen.cy + 1) + ";" +
                      std::to_string(screen.cx + 1) + "R";
}
}  // namespace

auto SCREEN::mode(Screen &screen, Byte final, const Vector<Whole> &values)
  -> Flag {
  const Byte prefix = screen.params.empty() ? 0 : screen.params.front();
  if (prefix == PRIVATE && (final == SET || final == RESET)) {
    for (const Whole code : values) dec(screen, code, final == SET);
    return true;
  }
  if (final == ATTRIBUTES) {
    if (prefix != SECONDARY) screen.replies += IDENTITY;
    return true;
  }
  if (final == STATUS) return status(screen, values.front()), true;
  return false;
}
