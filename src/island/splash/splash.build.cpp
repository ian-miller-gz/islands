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
#include <island/splash/splash.internal.hpp>

namespace {
using namespace GUI;

constexpr STRING::Hot LAYOUT =
  "style=assets/islands.gui\n"
  "style veil color=#00000000 edge=#00000000 border=0 hover=#00000000"
  " press=#00000000\n"
  "panel shell style=clear anchor=stretch\n"
  "  button band style=veil anchor=left|right x=0 y=0 w=0 h=24\n"
  "  button shade style=chrome size=18 text=\"◐\" anchor=northeast x=-144"
  " y=6 w=32 h=28\n"
  "  button shrink style=chrome size=18 text=\"—\" anchor=northeast x=-92"
  " y=6 w=32 h=28\n"
  "  button swell style=chrome size=18 text=\"□\" anchor=northeast x=-52"
  " y=6 w=32 h=28\n"
  "  button leave style=chrome size=18 text=\"x\" anchor=northeast x=-12"
  " y=6 w=32 h=28\n";
}  // namespace

auto SPLASH::compose() -> GUI::Handle {
  const String layout = LAYOUT;
  return GUI::load(SOURCE, Vector<Byte>(layout.begin(), layout.end()));
}
