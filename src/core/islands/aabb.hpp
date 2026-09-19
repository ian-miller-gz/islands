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
#include <islands/tilemap.hpp>

namespace ISLANDS::AABB {

struct Box {
  Float x = 0.0f;
  Float y = 0.0f;
  Float width = 0.0f;
  Float height = 0.0f;
};

inline auto overlap(const Box &a, const Box &b) -> Flag {
  return a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height &&
         a.y + a.height > b.y;
}

inline auto contains(const Box &box, Float px, Float py) -> Flag {
  return px >= box.x && px < box.x + box.width && py >= box.y &&
         py < box.y + box.height;
}

inline auto overlap(const Box &box, const TILEMAP::Grid &grid) -> Flag {
  if (grid.width == 0 || grid.height == 0) return false;
  const Float farX = box.x + box.width, farY = box.y + box.height;
  if (farX <= 0.0f || farY <= 0.0f) return false;
  const Whole column0 =
    box.x < 0.0f ? 0u : static_cast<Whole>(box.x) / grid.width;
  const Whole row0 =
    box.y < 0.0f ? 0u : static_cast<Whole>(box.y) / grid.height;
  Whole column1 = static_cast<Whole>(farX) / grid.width + 1;
  Whole row1 = static_cast<Whole>(farY) / grid.height + 1;
  if (column1 > grid.columns) column1 = grid.columns;
  if (row1 > grid.rows) row1 = grid.rows;
  for (Whole row = row0; row < row1; row += 1)
    for (Whole column = column0; column < column1; column += 1) {
      const TILEMAP::Tile *cell = TILEMAP::tile(grid, column, row);
      if (
        cell && cell->solid &&
        overlap(
          box, Box{
                 Float(column * grid.width), Float(row * grid.height),
                 Float(grid.width), Float(grid.height)}))
        return true;
    }
  return false;
}

}  // namespace ISLANDS::AABB
