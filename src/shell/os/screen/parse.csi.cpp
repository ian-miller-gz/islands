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

namespace SCREEN = SHELL::OS::SCREEN;

namespace {
constexpr Byte ESCAPE = 0x1b, BELL = 0x07, FINAL = 0x40, PRINTABLE = 0x20;

void ignore(SCREEN::Screen &screen, Byte) { screen.state = SCREEN::ground; }
}  // namespace

void SCREEN::escaped(Screen &screen, Byte byte) {
  screen.params.clear();
  if (byte == '[') return (void)(screen.state = csi);
  if (byte == ']') return (void)(screen.state = osc);
  if (byte == '(' || byte == ')' || byte == '*' || byte == '+')
    return (void)(screen.state = ignore);
  screen.state = ground;
}

void SCREEN::csi(Screen &screen, Byte byte) {
  if (byte >= FINAL && byte < 0x7f) {
    execute(screen, byte);
    screen.state = ground;
    return;
  }
  if (byte >= PRINTABLE) screen.params.push_back(static_cast<char>(byte));
}

void SCREEN::osc(Screen &screen, Byte byte) {
  if (byte == BELL) return (void)(screen.state = ground);
  if (byte == ESCAPE) return (void)(screen.state = ignore);
}
