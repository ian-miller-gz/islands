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
constexpr Whole RESET = 0, BOLD = 1, UNDERLINE = 4, REVERSE = 7;
constexpr Whole PLAINLY = 22, BARE = 24, UPRIGHT = 27;
constexpr Whole SPAN = 7;
namespace FG {
constexpr Whole BASE = 30, BRIGHT = 90, EXTENDED = 38, DEFAULT = 39;
}  // namespace FG
namespace BG {
constexpr Whole BASE = 40, BRIGHT = 100, EXTENDED = 48, DEFAULT = 49;
}  // namespace BG

auto attribute(SCREEN::Pen &pen, Whole code) -> Flag {
  if (code == RESET) return pen = {}, true;
  if (code == BOLD) return pen.bold = true, true;
  if (code == PLAINLY) return pen.bold = false, true;
  if (code == UNDERLINE) return pen.underline = true, true;
  if (code == BARE) return pen.underline = false, true;
  if (code == REVERSE) return pen.reverse = true, true;
  if (code == UPRIGHT) return pen.reverse = false, true;
  return false;
}

void colour(SCREEN::Pen &pen, const Vector<Whole> &values, Whole &index) {
  const Whole code = values[index];
  if (code == FG::DEFAULT)
    pen.fg = SCREEN::FOREGROUND, pen.base = SCREEN::PLAIN;
  if (code == BG::DEFAULT) pen.bg = SCREEN::BACKGROUND;
  if (code >= FG::BASE && code <= FG::BASE + SPAN)
    pen.base = code - FG::BASE, pen.fg = SCREEN::PALETTE[pen.base];
  if (code >= FG::BRIGHT && code <= FG::BRIGHT + SPAN)
    pen.fg = SCREEN::PALETTE[SCREEN::BRIGHT + code - FG::BRIGHT],
    pen.base = SCREEN::PLAIN;
  if (code >= BG::BASE && code <= BG::BASE + SPAN)
    pen.bg = SCREEN::PALETTE[code - BG::BASE];
  if (code >= BG::BRIGHT && code <= BG::BRIGHT + SPAN)
    pen.bg = SCREEN::PALETTE[SCREEN::BRIGHT + code - BG::BRIGHT];
  if (code == FG::EXTENDED && SCREEN::extended(values, index, pen.fg))
    pen.base = SCREEN::PLAIN;
  if (code == BG::EXTENDED) SCREEN::extended(values, index, pen.bg);
}
}  // namespace

void SCREEN::sgr(Screen &screen, const Vector<Whole> &values) {
  for (Whole index = 0; index < values.size(); index += 1)
    if (!attribute(screen.pen, values[index]))
      colour(screen.pen, values, index);
}

auto SCREEN::cell(const Screen &screen) -> Cell {
  const Pen &pen = screen.pen;
  GFX::Color fg =
    pen.bold && pen.base != PLAIN ? PALETTE[BRIGHT + pen.base] : pen.fg;
  GFX::Color bg = pen.bg;
  if (pen.reverse) std::swap(fg, bg);
  return {' ', fg, bg, pen.underline};
}
