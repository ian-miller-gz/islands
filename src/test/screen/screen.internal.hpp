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

#include <shell/os/screen/screen.internal.hpp>
#include <iostream>

namespace SCREEN = SHELL::OS::SCREEN;

inline Flag ok = true;

inline auto cell(SCREEN::Screen &screen, Whole col, Whole row)
  -> SCREEN::Cell & {
  return screen.cells[row * screen.cols + col];
}

inline void check(Flag pass, const char *what) {
  if (!pass) std::cout << "  FAIL: " << what << std::endl;
  ok = ok && pass;
}

inline auto hue(const GFX::Color &a, const GFX::Color &b) -> Flag {
  return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

void pen();

void modes();
