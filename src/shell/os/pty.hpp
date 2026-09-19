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
#pragma once

#include <common.hpp>
#include <common/platform/selection.hpp>

namespace SHELL::OS {

#if SR_PLATFORM == SR_WINDOWS
struct Pty {
  void *child = nullptr;
  void *console = nullptr;
  void *in = nullptr;
  void *out = nullptr;
};
#else
struct Pty {
  int child = -1;
  int master = -1;
};
#endif

namespace PTY {
constexpr Whole COLUMNS = 80;
constexpr Whole ROWS = 24;

constexpr Whole WIDEST = 240;
constexpr Whole TALLEST = 72;

#if SR_PLATFORM != SR_WEB
void resize(Pty &pty, Whole cols, Whole rows);

constexpr STRING::Hot TERM = "xterm-256color";

void spawn(Pty &pty);

Flag running(const Pty &pty);

void feed(Pty &pty, const String &bytes);

auto drain(Pty &pty) -> String;

void stop(Pty &pty);
#else
inline void resize(Pty &, Whole, Whole) {}
inline void spawn(Pty &) {}
inline Flag running(const Pty &) { return false; }
inline void feed(Pty &, const String &) {}
inline auto drain(Pty &) -> String { return {}; }
inline void stop(Pty &) {}
#endif
}  // namespace PTY

}  // namespace SHELL::OS
