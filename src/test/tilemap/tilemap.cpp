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
#include <common.hpp>
#include <iostream>
#include <islands/tilemap.hpp>

namespace TILEMAP = ISLANDS::TILEMAP;

namespace {

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

constexpr Whole COLUMNS = 8, CELL = 16;
constexpr STRING::Hot LINES[] = {
  "#......#", "#..##..#", "#......#", "#.o....#", "########"};

auto build() -> TILEMAP::Grid {
  const Vector<TILEMAP::Code> table = {
    {'#', {0, 0, true}}, {'o', {1, 0, false}}};
  const Vector<STRING::Hot> lines(LINES, LINES + 5);
  return TILEMAP::decode(lines, COLUMNS, CELL, CELL, table);
}

auto tiles(const TILEMAP::Grid &grid, const TILEMAP::Window &view) -> Whole {
  Whole seen = 0;
  for (Whole row = view.row0; row < view.row1; row += 1)
    for (Whole col = view.column0; col < view.column1; col += 1)
      if (TILEMAP::tile(grid, col, row)) seen += 1;
  return seen;
}

}  // namespace

auto main() -> int {
  const TILEMAP::Grid grid = build();
  Status status = 0;

  status |= check(
    "dims 8x5 cell 16", grid.columns == 8 && grid.rows == 5 &&
                          grid.width == 16 && grid.height == 16);
  status |= check("solid at (56,24) block", TILEMAP::solid(grid, 56, 24));
  status |= check("open at (32,0) gap", !TILEMAP::solid(grid, 32, 0));
  status |= check("open at (40,52) marker", !TILEMAP::solid(grid, 40, 52));
  status |= check("solid at (112,72) floor", TILEMAP::solid(grid, 112, 72));
  status |= check("open past edge (200,8)", !TILEMAP::solid(grid, 200, 8));

  status |= check("surface col0 = 0 wall", TILEMAP::surface(grid, 0) == 0);
  status |= check("surface col3 = 1 block", TILEMAP::surface(grid, 3) == 1);
  status |= check("surface col5 = 4 floor", TILEMAP::surface(grid, 5) == 4);

  const TILEMAP::Camera near = TILEMAP::follow(grid, 8, 8, 64, 64);
  status |=
    check("follow left/top clamp 0,0", near.x == 0.0f && near.y == 0.0f);
  const TILEMAP::Camera far = TILEMAP::follow(grid, 999, 999, 64, 64);
  status |=
    check("follow right/bot clamp 64,16", far.x == 64.0f && far.y == 16.0f);
  const TILEMAP::Camera mid = TILEMAP::follow(grid, 96, 40, 64, 32);
  status |= check("follow interior 64,24", mid.x == 64.0f && mid.y == 24.0f);

  const TILEMAP::Window corner = TILEMAP::window(grid, 0, 0, 32, 32);
  status |= check(
    "window (0,0) 32x32 -> [0,0 2,2) tiles 2",
    corner.column0 == 0 && corner.row0 == 0 && corner.column1 == 2 &&
      corner.row1 == 2 && tiles(grid, corner) == 2);
  const TILEMAP::Window scrolled = TILEMAP::window(grid, 48, 16, 32, 32);
  status |= check(
    "window (48,16) 32x32 -> [3,1 5,3) tiles 2",
    scrolled.column0 == 3 && scrolled.row0 == 1 && scrolled.column1 == 5 &&
      scrolled.row1 == 3 && tiles(grid, scrolled) == 2);
  const TILEMAP::Window all = TILEMAP::window(grid, 0, 0, 128, 80);
  status |= check(
    "window whole map tiles 19",
    all.column1 == 8 && all.row1 == 5 && tiles(grid, all) == 19);

  return status;
}
