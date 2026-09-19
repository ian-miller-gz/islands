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
#include <island/graphics/sprites/sprites.hpp>
#include "tilemap.hpp"

namespace ISLANDS::TILEMAP {

inline void draw(
  const Grid &grid, GFX::SPRITES::Handle atlas, GFX::SPRITES::Handle batch,
  Float camX = 0.0f, Float camY = 0.0f, Whole viewWidth = 0,
  Whole viewHeight = 0) {
  const Window view = (viewWidth == 0 && viewHeight == 0)
                        ? Window{0, 0, grid.columns, grid.rows}
                        : window(grid, camX, camY, viewWidth, viewHeight);
  for (Whole row = view.row0; row < view.row1; row += 1)
    for (Whole column = view.column0; column < view.column1; column += 1) {
      const Tile *cell = tile(grid, column, row);
      if (!cell) continue;
      GFX::SPRITES::cell(
        batch, Float(column * grid.width), Float(row * grid.height),
        Float(grid.width), Float(grid.height), atlas, cell->column, cell->row);
    }
}

}  // namespace ISLANDS::TILEMAP
