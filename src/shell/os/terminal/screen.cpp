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
#include <shell/os/screen.hpp>
#include <shell/os/terminal/render.internal.hpp>
#include <shell/os/screen/screen.internal.hpp>
#include <shell/os/keys.hpp>
#include <shell/os/pty.hpp>
#include <utility>

namespace SCREEN = SHELL::OS::SCREEN;

static SCREEN::Screen grid;
static SHELL::OS::Pty child;

auto SCREEN::GET::grid() -> Screen & { return ::grid; }

void SCREEN::open() {
  if (SHELL::OS::PTY::running(child)) return;
  SHELL::OS::PTY::spawn(child);
  wipe(grid, SHELL::OS::PTY::COLUMNS, SHELL::OS::PTY::ROWS);
}

auto SCREEN::running() -> Flag { return SHELL::OS::PTY::running(child); }

void SCREEN::fit(Whole cols, Whole rows) {
  if (grid.cols == cols && grid.rows == rows) return;
  resize(grid, cols, rows);
  SHELL::OS::PTY::resize(child, cols, rows);
}

void SCREEN::process() {
  if (!SHELL::OS::PTY::running(child)) return;
  feed(grid, SHELL::OS::PTY::drain(child));
  if (!grid.replies.empty())
    SHELL::OS::PTY::feed(child, std::exchange(grid.replies, {}));
}

void SCREEN::feed(const Vector<INPUT::KEYS::Event> &events) {
  if (!SHELL::OS::PTY::running(child)) return;
  for (const auto &event : events)
    SHELL::OS::PTY::feed(child, KEYS::encode(event, grid.cursors));
}

void SCREEN::close() {
  SHELL::OS::PTY::stop(child);
  drop();
}
