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

void modes() {
  SCREEN::Screen s;

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "keep\x1b[?1049h");
  check(cell(s, 0, 0).glyph == ' ' && s.cx == 0 && s.cy == 0, "alt.blank");
  SCREEN::feed(s, "over\x1b[?1049l");
  check(
    cell(s, 0, 0).glyph == 'k' && cell(s, 3, 0).glyph == 'p', "alt.restore");
  check(s.cx == 4 && s.cy == 0, "alt.cursor");

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "\x1b[?25l");
  check(!s.cursor, "mode.cursor.off");
  SCREEN::feed(s, "\x1b[?25h\x1b[?1h");
  check(s.cursor && s.cursors, "mode.cursor.on");
  SCREEN::feed(s, "\x1b[?1l");
  check(!s.cursors, "mode.arrows.off");

  SCREEN::wipe(s, 8, 3);
  SCREEN::feed(s, "\x1b[2;3H\x1b[6n");
  check(s.replies == "\x1b[2;3R", "report.position");
  s.replies.clear();
  SCREEN::feed(s, "\x1b[5n\x1b[c");
  check(s.replies == "\x1b[0n\x1b[?1;2c", "report.status");

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "abcdef\x1b[H\x1b[2@");
  check(
    cell(s, 0, 0).glyph == ' ' && cell(s, 2, 0).glyph == 'a', "edit.insert");

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "abcdef\x1b[H\x1b[2P");
  check(
    cell(s, 0, 0).glyph == 'c' && cell(s, 7, 0).glyph == ' ', "edit.delete");

  SCREEN::wipe(s, 8, 2);
  SCREEN::feed(s, "abcdef\x1b[H\x1b[2X");
  check(cell(s, 0, 0).glyph == ' ' && cell(s, 2, 0).glyph == 'c', "edit.erase");

  SCREEN::wipe(s, 4, 3);
  SCREEN::feed(s, "aa\r\nbb\x1b[H\x1b[L");
  check(cell(s, 0, 0).glyph == ' ' && cell(s, 0, 1).glyph == 'a', "edit.open");
  SCREEN::feed(s, "\x1b[H\x1b[M");
  check(cell(s, 0, 0).glyph == 'a' && cell(s, 0, 1).glyph == 'b', "edit.drop");
}
