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
auto at(const Vector<Whole> &v, Whole i) -> Whole {
  return i < v.size() ? v[i] : 0;
}
auto one(const Vector<Whole> &v, Whole i) -> Whole {
  const Whole n = at(v, i);
  return n ? n : 1;
}
}  // namespace

void SCREEN::motion(Screen &s, Byte final, const Vector<Whole> &v) {
  const Whole n = one(v, 0), wall = s.cols - 1, floor = s.rows - 1;
  if (final == 'A')
    s.cy -= std::min(s.cy, n);
  else if (final == 'B')
    s.cy = std::min<Whole>(floor, s.cy + n);
  else if (final == 'C')
    s.cx = std::min<Whole>(wall, s.cx + n);
  else if (final == 'D')
    s.cx -= std::min(s.cx, n);
  else if (final == 'E')
    s.cy = std::min<Whole>(floor, s.cy + n), s.cx = 0;
  else if (final == 'F')
    s.cy -= std::min(s.cy, n), s.cx = 0;
  else if (final == 'G')
    s.cx = std::min<Whole>(wall, n - 1);
  else if (final == 'd')
    s.cy = std::min<Whole>(floor, n - 1);
  else if (final == 'H' || final == 'f')
    s.cy = std::min<Whole>(floor, n - 1),
    s.cx = std::min<Whole>(wall, one(v, 1) - 1);
}
