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
#include <island/graphics/sprites/sprites.internal.hpp>
#include <island/graphics/backend/resources.hpp>
#include <island/graphics/images/images.hpp>
#include <logger.hpp>
#include <string>
#define LOGGER_CATEGORY "~/island/graphics/sprites::"

namespace STATE = GFX::SPRITES::STATE;
namespace ATLASES = GFX::SPRITES::ATLASES;

auto GFX::SPRITES::ATLASES::create(
  Whole width, Whole height, Cell cell, const Vector<Byte> &rgba) -> Handle {
  if (STATE::sampler == GFX::NONE) {
    STATE::sampler = GFX::SAMPLERS::create(GFX::SAMPLERS::CLAMP);
  }
  const auto texture = GFX::TEXTURES::create(width, height);
  GFX::TEXTURES::write(texture, rgba.data());
  STATE::atlases.push_back({texture, width, height, cell});
  return static_cast<Handle>(STATE::atlases.size() - 1);
}

auto ATLASES::create(STRING::Hot path, Cell cell) -> Handle {
  const auto image = GFX::IMAGES::load(path);
  if (image.rgba.empty()) return NONE;
  return create(image.width, image.height, cell, image.rgba);
}

static void stamp(
  Vector<Byte> &rgba, Whole wide, ATLASES::Cell cell, Whole column,
  const ATLASES::Mask &mask) {
  for (Whole v = 0; v < cell.height; v += 1)
    for (Whole u = 0; u < cell.width; u += 1) {
      if (!mask.coverage[v * cell.width + u]) continue;
      Byte *texel =
        &rgba[(v * wide + column * cell.width + u) * GFX::TEXTURES::STRIDE];
      texel[0] = texel[1] = texel[2] = texel[3] = static_cast<Byte>(255);
    }
}

auto ATLASES::create(Cell cell, const Vector<Mask> &cells) -> Handle {
  const Whole span = cell.width * cell.height;
  for (Whole column = 0; column < cells.size(); column += 1)
    if (cells[column].coverage.size() != span) {
      LOGGER::get(LOGGER_CATEGORY "atlases")
        .error(
          "Mask cell " + std::to_string(column) + " covers " +
          std::to_string(cells[column].coverage.size()) + " bytes; the " +
          std::to_string(cell.width) + "x" + std::to_string(cell.height) +
          " grid needs " + std::to_string(span) + ".");
      return NONE;
    }
  const Whole wide = cell.width * static_cast<Whole>(cells.size());
  Vector<Byte> rgba(wide * cell.height * GFX::TEXTURES::STRIDE, 0);
  for (Whole column = 0; column < cells.size(); column += 1)
    ::stamp(rgba, wide, cell, column, cells[column]);
  return create(wide, cell.height, cell, rgba);
}

auto GFX::SPRITES::ATLASES::GET::extent(Handle atlas) -> Extent {
  const auto &record = STATE::atlases[atlas];
  return {record.width, record.height};
}

auto GFX::SPRITES::ATLASES::GET::sampling(Handle atlas) -> GFX::Sampling {
  return {STATE::atlases[atlas].texture, STATE::sampler};
}

auto GFX::SPRITES::ATLASES::frame(Handle atlas, Whole column, Whole row)
  -> Frame {
  const auto &record = STATE::atlases[atlas];
  if (!record.cell.width || !record.cell.height) {
    return {0.0f, 0.0f, 1.0f, 1.0f};
  }
  const auto wide = static_cast<Float>(record.width);
  const auto tall = static_cast<Float>(record.height);
  return {
    static_cast<Float>(column * record.cell.width) / wide,
    static_cast<Float>(row * record.cell.height) / tall,
    static_cast<Float>(record.cell.width) / wide,
    static_cast<Float>(record.cell.height) / tall};
}
