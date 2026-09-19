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

#include <shell/os/screen/screen.palette.internal.hpp>
#include <common.hpp>

namespace SHELL::OS::SCREEN {

struct Screen;
using State = void (*)(Screen &, Byte);
void ground(Screen &, Byte);
void escaped(Screen &, Byte);
void csi(Screen &, Byte);
void osc(Screen &, Byte);

void execute(Screen &screen, Byte final);

void motion(Screen &screen, Byte final, const Vector<Whole> &values);

void sgr(Screen &screen, const Vector<Whole> &values);

auto cell(const Screen &screen) -> Cell;

auto edited(Screen &screen, Byte final, Whole count) -> Flag;

auto mode(Screen &screen, Byte final, const Vector<Whole> &values) -> Flag;

struct Screen {
  Whole cols = 0, rows = 0;
  Vector<Cell> cells;
  Whole cx = 0, cy = 0;
  Pen pen;
  State state = ground;
  String params;
  String replies;
  Flag cursor = true;
  Flag cursors = false;
  Flag alternate = false;
  Vector<Cell> held;
  Whole hx = 0, hy = 0;
};

void wipe(Screen &screen, Whole cols, Whole rows);

void resize(Screen &screen, Whole cols, Whole rows);

void feed(Screen &screen, const String &bytes);

namespace GET {
auto grid() -> Screen &;
}  // namespace GET

}  // namespace SHELL::OS::SCREEN
