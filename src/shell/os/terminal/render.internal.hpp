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
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/text/text.hpp>

namespace SHELL::OS::SCREEN {

constexpr Float MARGIN = 12;

struct Metrics {
  Float x = 0, y = 0;
  Float cell = 0;
  Float line = 0;
  Whole cols = 0, rows = 0;
  Float scale = 1;
  Float band = 0;
};

inline auto same(const GFX::Color &a, const GFX::Color &b) -> Flag {
  return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

auto place(const GFX::Pass &pass) -> Metrics;

void fit(Whole cols, Whole rows);

void quad(Float x, Float y, Float w, Float h, const GFX::Color &color);

void cells(const Metrics &metrics);

void glyphs(GFX::Pass &pass, const Metrics &metrics);

void drop();

}  // namespace SHELL::OS::SCREEN
