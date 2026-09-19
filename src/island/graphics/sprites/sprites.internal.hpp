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

namespace GFX::SPRITES {
inline const String SHADERS = COMMON::ASSETS::SHADERS + "sprites";
constexpr Whole VERTICES = 6;
constexpr Whole CAMERA = 0;

struct Batch {
  GFX::Handle buffer = GFX::NONE;
  GFX::Handle pipeline = GFX::NONE;
  Handle atlas = 0;
  Whole limit = 0;
  Whole count = 0;
  Flag snap = false;
};

struct Named {
  String name;
  ATLASES::Placement placement;
};

struct Atlas {
  GFX::Handle texture = GFX::NONE;
  Whole width = 0;
  Whole height = 0;
  ATLASES::Cell cell;
  Vector<Named> frames;
};

auto descriptor(STRING::Hot path) -> Handle;
}  // namespace GFX::SPRITES

namespace GFX::SPRITES::STATE {
extern Vector<Batch> batches;
extern Vector<Atlas> atlases;
extern GFX::Handle sampler;
extern Handle white;
}  // namespace GFX::SPRITES::STATE
