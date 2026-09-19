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
#include <test/screen/screen.internal.hpp>

void pen() {
  SCREEN::Screen s;

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "\x1b[1m\x1b[32mB");
  check(hue(cell(s, 0, 0).fg, SCREEN::PALETTE[SCREEN::BRIGHT + 2]), "pen.bold");

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "\x1b[1;32mB\x1b[22mP");
  check(hue(cell(s, 0, 0).fg, SCREEN::PALETTE[SCREEN::BRIGHT + 2]), "pen.lit");
  check(hue(cell(s, 1, 0).fg, SCREEN::PALETTE[2]), "pen.plain");

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "\x1b[31m\x1b[7mR\x1b[27mN");
  check(hue(cell(s, 0, 0).fg, SCREEN::BACKGROUND), "pen.reverse.fg");
  check(hue(cell(s, 0, 0).bg, SCREEN::PALETTE[1]), "pen.reverse.bg");
  check(hue(cell(s, 1, 0).fg, SCREEN::PALETTE[1]), "pen.upright.fg");
  check(hue(cell(s, 1, 0).bg, SCREEN::BACKGROUND), "pen.upright.bg");

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "\x1b[38;5;2mA\x1b[38;5;196mB\x1b[38;5;244mC");
  check(hue(cell(s, 0, 0).fg, SCREEN::PALETTE[2]), "pen.indexed.palette");
  check(hue(cell(s, 1, 0).fg, SCREEN::indexed(196)), "pen.indexed.cube");
  check(cell(s, 1, 0).fg.r == 1.0f && cell(s, 1, 0).fg.g == 0.0f, "pen.cube");
  check(
    cell(s, 2, 0).fg.r == cell(s, 2, 0).fg.b && cell(s, 2, 0).fg.r > 0.0f,
    "pen.grey");

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "\x1b[38;2;255;0;0mR\x1b[38:5:4mB");
  check(cell(s, 0, 0).fg.r == 1.0f && cell(s, 0, 0).fg.g == 0.0f, "pen.direct");
  check(hue(cell(s, 1, 0).fg, SCREEN::PALETTE[4]), "pen.colons");

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "\x1b[0;38;5;76;48;5;238;4mX");
  check(hue(cell(s, 0, 0).fg, SCREEN::indexed(76)), "pen.compound.fg");
  check(hue(cell(s, 0, 0).bg, SCREEN::indexed(238)), "pen.compound.bg");
  check(cell(s, 0, 0).underline, "pen.compound.underline");

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "\x1b[1;4;7;31m\x1b[0mN");
  check(hue(cell(s, 0, 0).fg, SCREEN::FOREGROUND), "pen.reset.fg");
  check(hue(cell(s, 0, 0).bg, SCREEN::BACKGROUND), "pen.reset.bg");
  check(!cell(s, 0, 0).underline, "pen.reset.underline");

  SCREEN::wipe(s, 16, 2);
  SCREEN::feed(
    s,
    "\x1b[0m\x1b[32m\x1b[40m\x1b[32mok\x1b[0m\x1b[37m\x1b[47m\x1b[30m 06"
    "\x1b[0m\x1b[30m\x1b[47m\x1b[49m\x1b[39m\x1b[38;5;238m~\x1b[0m");
  Flag legible = true;
  for (Whole col = 0; col < 6; col += 1)
    legible = legible && !hue(cell(s, col, 0).fg, cell(s, col, 0).bg);
  check(legible, "pen.prompt.legible");
}
