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
constexpr Byte ESCAPE = 0x1b, BELL = 0x07, TAB = 8;

void scroll(SCREEN::Screen &screen) {
  screen.cells.erase(screen.cells.begin(), screen.cells.begin() + screen.cols);
  screen.cells.insert(screen.cells.end(), screen.cols, SCREEN::Cell{});
}

void linefeed(SCREEN::Screen &screen) {
  if (screen.cy + 1 < screen.rows)
    screen.cy += 1;
  else
    scroll(screen);
}

void put(SCREEN::Screen &screen, Whole glyph) {
  if (screen.cells.empty()) return;
  auto cell = SCREEN::cell(screen);
  cell.glyph = glyph;
  screen.cells[screen.cy * screen.cols + screen.cx] = cell;
  screen.cx += 1;
  if (screen.cx >= screen.cols) {
    screen.cx = 0;
    linefeed(screen);
  }
}
}  // namespace

void SCREEN::ground(Screen &screen, Byte byte) {
  if (byte == ESCAPE) return (void)(screen.state = escaped);
  if (byte == '\r') return (void)(screen.cx = 0);
  if (byte == '\n' || byte == '\v' || byte == '\f') return linefeed(screen);
  if (byte == '\b') return (void)(screen.cx -= screen.cx ? 1 : 0);
  if (byte == '\t')
    return (void)(screen.cx = std::min<Whole>(
                    screen.cols - 1, (screen.cx / TAB + 1) * TAB));
  if (byte == BELL || byte < ' ') return;
  if (byte < 0x7f) put(screen, byte);
}

void SCREEN::wipe(Screen &screen, Whole cols, Whole rows) {
  screen.cols = cols;
  screen.rows = rows;
  screen.cells.assign(cols * rows, Cell{});
  screen.cx = screen.cy = 0;
  screen.pen = {};
  screen.state = ground;
  screen.params.clear();
  screen.replies.clear();
  screen.cursor = true;
  screen.cursors = screen.alternate = false;
  screen.held.clear();
  screen.hx = screen.hy = 0;
}

void SCREEN::feed(Screen &screen, const String &bytes) {
  for (const char byte : bytes) screen.state(screen, static_cast<Byte>(byte));
}
