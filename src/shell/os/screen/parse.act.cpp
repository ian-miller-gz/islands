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
auto digits(const String &params) -> Vector<Whole> {
  Vector<Whole> values{0};
  for (const char byte : params) {
    if (byte >= '0' && byte <= '9')
      values.back() = values.back() * 10 + static_cast<Whole>(byte - '0');
    else if (byte == ';' || byte == ':')
      values.push_back(0);
  }
  return values;
}
auto at(const Vector<Whole> &v, Whole i) -> Whole {
  return i < v.size() ? v[i] : 0;
}
auto count(const Vector<Whole> &v) -> Whole {
  const Whole n = at(v, 0);
  return n ? n : 1;
}
void blank(SCREEN::Screen &s, Whole from, Whole to) {
  for (Whole i = from; i < to && i < s.cells.size(); i += 1) s.cells[i] = {};
}
void erase(SCREEN::Screen &s, Byte final, Whole mode) {
  const Whole cur = s.cy * s.cols + s.cx, line = s.cy * s.cols;
  const Whole eol = line + s.cols, full = s.cols * s.rows;
  if (final == 'K')
    blank(s, mode == 1 ? line : cur, mode == 0 ? eol : cur + 1);
  else if (mode == 0)
    blank(s, cur, full);
  else
    blank(s, mode == 1 ? 0 : 0, mode == 1 ? cur + 1 : full);
}
void scroll(SCREEN::Screen &s, Byte final, Whole n) {
  const auto span = std::min<Whole>(n, s.rows) * s.cols;
  if (final == 'S') {
    s.cells.erase(s.cells.begin(), s.cells.begin() + span);
    s.cells.insert(s.cells.end(), span, SCREEN::Cell{});
  } else {
    s.cells.insert(s.cells.begin(), span, SCREEN::Cell{});
    s.cells.erase(s.cells.end() - static_cast<long>(span), s.cells.end());
  }
}
}  // namespace

void SCREEN::execute(Screen &s, Byte final) {
  if (s.cells.empty()) return;
  const auto v = digits(s.params);
  if (mode(s, final, v)) return;
  if (edited(s, final, count(v))) return;
  if (final == 'J' || final == 'K')
    erase(s, final, at(v, 0));
  else if (final == 'S' || final == 'T')
    scroll(s, final, count(v));
  else if (final == 'm')
    sgr(s, v);
  else
    motion(s, final, v);
}
