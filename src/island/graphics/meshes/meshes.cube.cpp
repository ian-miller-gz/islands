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
#include <island/graphics/backend/resources.hpp>
#include <island/graphics/math.hpp>
#include <island/graphics/meshes/meshes.hpp>
#include <cstring>

namespace {
constexpr Whole FACES = 6;
constexpr Whole SIDES = 4;
constexpr Whole POINTS = FACES * SIDES;
constexpr Whole INDICES = FACES * 6;
constexpr Float HALF = 0.5f;

const Float NORMALS[FACES][3] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                                 {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};

void corner(Float *vertex, const Float normal[3], Float u, Float v) {
  const Whole main = normal[0] != 0.0f ? 0 : normal[1] != 0.0f ? 1 : 2;
  const Whole from = (main + 1) % 3;
  const Whole to = (main + 2) % 3;
  vertex[main] = normal[main] * HALF;
  vertex[from] = (u - HALF) * normal[main] * 2.0f * HALF;
  vertex[to] = (v - HALF) * 2.0f * HALF;
  std::memcpy(vertex + 3, normal, 3 * sizeof(Float));
  vertex[6] = u;
  vertex[7] = v;
}

void fill(Float *floats, Word *order) {
  for (Whole face = 0; face < FACES; face += 1) {
    auto *quad = floats + face * SIDES * GFX::MODEL::LAYOUT::STRIDE;
    corner(quad, NORMALS[face], 0.0f, 0.0f);
    corner(quad + GFX::MODEL::LAYOUT::STRIDE, NORMALS[face], 1.0f, 0.0f);
    corner(quad + 2 * GFX::MODEL::LAYOUT::STRIDE, NORMALS[face], 1.0f, 1.0f);
    corner(quad + 3 * GFX::MODEL::LAYOUT::STRIDE, NORMALS[face], 0.0f, 1.0f);
    const Word base = static_cast<Word>(face * SIDES);
    const Word wound[6] = {
      base, static_cast<Word>(base + 1), static_cast<Word>(base + 2),
      base, static_cast<Word>(base + 2), static_cast<Word>(base + 3)};
    std::memcpy(order + face * 6, wound, sizeof(wound));
  }
}
}  // namespace

auto GFX::MESHES::cube() -> GFX::MODEL::Scene {
  GFX::MODEL::Scene scene;
  GFX::MODEL::Mesh mesh;
  constexpr Whole SIZE = POINTS * GFX::MODEL::LAYOUT::STRIDE * sizeof(Float);
  mesh.vertices = GFX::BUFFERS::create(SIZE, GFX::BUFFERS::VERTICES);
  mesh.indices =
    GFX::BUFFERS::create(INDICES * sizeof(Word), GFX::BUFFERS::INDICES);
  auto *floats = GFX::BUFFERS::GET::data<Float>(mesh.vertices);
  std::memset(floats, 0, SIZE);
  fill(floats, GFX::BUFFERS::GET::data<Word>(mesh.indices));
  mesh.count = INDICES;
  scene.meshes.push_back(mesh);
  GFX::MODEL::Node node;
  node.name = "Cube";
  GFX::MATH::reset(node.local);
  GFX::MATH::reset(node.world);
  node.count = 1;
  scene.nodes.push_back(node);
  scene.roots.push_back(0);
  for (Whole axis = 0; axis < 3; axis += 1) {
    scene.bounds.min[axis] = -HALF;
    scene.bounds.max[axis] = HALF;
  }
  return scene;
}
