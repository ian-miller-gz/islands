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
#include <common.hpp>

namespace ISLANDS::TILEMAP {

constexpr Whole NONE = static_cast<Whole>(-1);

struct Tile {
  Whole column = 0;
  Whole row = 0;
  Flag solid = false;
};

struct Code {
  char mark = 0;
  Tile tile;
};

struct Grid {
  Whole columns = 0;
  Whole rows = 0;
  Whole width = 0;
  Whole height = 0;
  Vector<Whole> cells;
  Vector<Tile> tiles;
};

struct Camera {
  Float x = 0.0f;
  Float y = 0.0f;
};

struct Window {
  Whole column0 = 0;
  Whole row0 = 0;
  Whole column1 = 0;
  Whole row1 = 0;
};

inline auto tile(const Grid &grid, Whole column, Whole row) -> const Tile * {
  if (column >= grid.columns || row >= grid.rows) return nullptr;
  const Whole code = grid.cells[row * grid.columns + column];
  return code == NONE ? nullptr : &grid.tiles[code];
}

inline auto solid(const Grid &grid, Float x, Float y) -> Flag {
  if (x < 0.0f || y < 0.0f || grid.width == 0 || grid.height == 0) return false;
  const Tile *cell = tile(
    grid, static_cast<Whole>(x) / grid.width,
    static_cast<Whole>(y) / grid.height);
  return cell && cell->solid;
}

inline auto surface(const Grid &grid, Whole column) -> Integer {
  for (Whole row = 0; row < grid.rows; row += 1) {
    const Tile *cell = tile(grid, column, row);
    if (cell && cell->solid) return static_cast<Integer>(row);
  }
  return -1;
}

inline auto follow(
  const Grid &grid, Float focusX, Float focusY, Whole viewWidth,
  Whole viewHeight) -> Camera {
  const Float mapW = Float(grid.columns * grid.width);
  const Float mapH = Float(grid.rows * grid.height);
  const Float restX = mapW - Float(viewWidth), restY = mapH - Float(viewHeight);
  Float x = focusX - Float(viewWidth) / 2.0f;
  Float y = focusY - Float(viewHeight) / 2.0f;
  x = x < 0.0f ? 0.0f : (x > restX ? restX : x);
  y = y < 0.0f ? 0.0f : (y > restY ? restY : y);
  return {restX < 0.0f ? 0.0f : x, restY < 0.0f ? 0.0f : y};
}

inline auto window(
  const Grid &grid, Float camX, Float camY, Whole viewWidth,
  Whole viewHeight) -> Window {
  const Integer left = static_cast<Integer>(camX) / Integer(grid.width);
  const Integer top = static_cast<Integer>(camY) / Integer(grid.height);
  const Whole right =
    (static_cast<Whole>(camX + Float(viewWidth)) + grid.width - 1) / grid.width;
  const Whole bottom =
    (static_cast<Whole>(camY + Float(viewHeight)) + grid.height - 1) /
    grid.height;
  return {
    left < 0 ? 0u : Whole(left), top < 0 ? 0u : Whole(top),
    right > grid.columns ? grid.columns : right,
    bottom > grid.rows ? grid.rows : bottom};
}

inline auto decode(
  const Vector<STRING::Hot> &lines, Whole columns, Whole width, Whole height,
  const Vector<Code> &table) -> Grid {
  Grid grid{columns, Whole(lines.size()), width, height};
  grid.cells.assign(grid.columns * grid.rows, NONE);
  for (const Code &code : table) grid.tiles.push_back(code.tile);
  for (Whole row = 0; row < grid.rows; row += 1)
    for (Whole column = 0; column < columns && lines[row][column]; column += 1)
      for (Whole index = 0; index < table.size(); index += 1)
        if (lines[row][column] == table[index].mark)
          grid.cells[row * columns + column] = index;
  return grid;
}

inline auto decode(
  const Vector<Whole> &cells, Whole columns, Whole rows, Whole width,
  Whole height, const Vector<Tile> &tiles) -> Grid {
  return {columns, rows, width, height, cells, tiles};
}

}  // namespace ISLANDS::TILEMAP
