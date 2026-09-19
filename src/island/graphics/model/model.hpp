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

#include <island/graphics/model/model.meshes.hpp>
#include <common.hpp>

namespace GFX::MODEL {

struct Node {
  String name;
  Float local[16] = {};
  Float world[16] = {};
  Whole first = 0;
  Whole count = 0;
  Vector<Whole> children;
};

struct Bounds {
  Float min[3] = {};
  Float max[3] = {};
};

struct Scene {
  Vector<Mesh> meshes;
  Vector<Node> nodes;
  Vector<Whole> roots;
  Bounds bounds;
};

auto load(const String &path, Scene &scene) -> Status;
void destroy(Scene &scene);

}  // namespace GFX::MODEL

namespace GFX::MODEL::LAYOUT {
constexpr Whole POSITION = 3;
constexpr Whole NORMAL = 3;
constexpr Whole TEXCOORD = 2;
constexpr Whole STRIDE = POSITION + NORMAL + TEXCOORD;

inline const Vector<Whole> ATTRIBUTES = {POSITION, NORMAL, TEXCOORD};
}  // namespace GFX::MODEL::LAYOUT
