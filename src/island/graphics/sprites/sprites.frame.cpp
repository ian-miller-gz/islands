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
#include <cmath>

namespace STATE = GFX::SPRITES::STATE;

void GFX::SPRITES::push(Handle batch, Sprite sprite) {
  auto &record = STATE::batches[batch];
  if (record.count == record.limit) return;
  if (record.snap) {
    sprite.x = std::floor(sprite.x);
    sprite.y = std::floor(sprite.y);
  }
  GFX::BUFFERS::GET::data<Sprite>(record.buffer)[record.count] = sprite;
  record.count += 1;
}

void GFX::SPRITES::flush(Handle batch, GFX::Pass &pass) {
  auto &record = STATE::batches[batch];
  const auto count = record.count;
  record.count = 0;
  if (!count) return;
  const auto &atlas = STATE::atlases[record.atlas];
  pass.draws.push_back(
    {.pipeline = record.pipeline,
     .vertex = {.buffers = {record.buffer}},
     .fragment = {.samplers = {{atlas.texture, STATE::sampler}}},
     .count = count * VERTICES});
}
