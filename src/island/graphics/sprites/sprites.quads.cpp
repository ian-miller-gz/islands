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

namespace STATE = GFX::SPRITES::STATE;

void GFX::SPRITES::push(
  Handle batch, Float x, Float y, Float w, Float h, Frame frame,
  GFX::Color color, Flag flipX, Flag flipY) {
  if (flipX) {
    frame.u += frame.w;
    frame.w = -frame.w;
  }
  if (flipY) {
    frame.v += frame.h;
    frame.h = -frame.h;
  }
  push(
    batch, {.x = x, .y = y, .w = w, .h = h, .texture = frame, .color = color});
}

void GFX::SPRITES::rect(
  Handle batch, Float x, Float y, Float w, Float h, GFX::Color color) {
  push(batch, {.x = x, .y = y, .w = w, .h = h, .color = color});
}

void GFX::SPRITES::cell(
  Handle batch, Float x, Float y, Float w, Float h, Handle atlas, Whole column,
  Whole row, GFX::Color color) {
  push(batch, x, y, w, h, ATLASES::frame(atlas, column, row), color);
}

auto GFX::SPRITES::ATLASES::white() -> Handle {
  if (STATE::white != NONE) return STATE::white;
  const Vector<Byte> texel(GFX::TEXTURES::STRIDE, static_cast<Byte>(255));
  STATE::white = create(1, 1, {0, 0}, texel);
  return STATE::white;
}
