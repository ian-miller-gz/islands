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
#include <iostream>

int main() {
  SCREEN::Screen s;

  SCREEN::wipe(s, 4, 3);
  SCREEN::feed(s, "abcde");
  check(cell(s, 0, 0).glyph == 'a' && cell(s, 3, 0).glyph == 'd', "wrap.row0");
  check(cell(s, 0, 1).glyph == 'e' && s.cx == 1 && s.cy == 1, "wrap.row1");

  SCREEN::wipe(s, 8, 3);
  SCREEN::feed(s, "ab\rXY\b\n");
  check(cell(s, 0, 0).glyph == 'X' && cell(s, 1, 0).glyph == 'Y', "c0.text");
  check(s.cx == 1 && s.cy == 1, "c0.cursor");

  SCREEN::wipe(s, 10, 3);
  SCREEN::feed(s, "hello\x1b[H\x1b[K");
  check(cell(s, 0, 0).glyph == ' ' && cell(s, 4, 0).glyph == ' ', "erase.line");
  check(s.cx == 0 && s.cy == 0, "erase.cursor");

  SCREEN::wipe(s, 10, 2);
  SCREEN::feed(s, "\x1b[31mR\x1b[0mN");
  check(
    cell(s, 0, 0).glyph == 'R' && hue(cell(s, 0, 0).fg, SCREEN::PALETTE[1]),
    "sgr.red");
  check(hue(cell(s, 1, 0).fg, SCREEN::FOREGROUND), "sgr.reset");

  SCREEN::wipe(s, 10, 4);
  SCREEN::feed(s, "\x1b[2;3H*");
  check(cell(s, 2, 1).glyph == '*' && s.cx == 3 && s.cy == 1, "cup");

  SCREEN::wipe(s, 10, 2);
  SCREEN::feed(s, "\x1b]0;title\x07Z");
  check(cell(s, 0, 0).glyph == 'Z' && s.cx == 1, "osc.swallow");

  SCREEN::wipe(s, 6, 3);
  SCREEN::feed(s, "ab\r\ncd\r\nef");
  SCREEN::resize(s, 4, 3);
  check(cell(s, 0, 0).glyph == 'a' && cell(s, 1, 2).glyph == 'f', "fit.wider");
  check(s.cols == 4 && s.rows == 3 && s.cx == 2 && s.cy == 2, "fit.extent");
  SCREEN::resize(s, 4, 2);
  check(cell(s, 0, 0).glyph == 'c' && cell(s, 0, 1).glyph == 'e', "fit.tail");
  check(s.cy == 1 && s.cx == 2, "fit.cursor");

  pen();
  modes();

  std::cout << "screen: " << (ok ? "ok" : "FAILED") << std::endl;
  return ok ? 0 : 1;
}
