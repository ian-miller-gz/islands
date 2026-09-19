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
using GFX::MODEL::LAYOUT::STRIDE;
constexpr Float HALF = 0.5f;

void corner(Float *vertex, Float x, Float z, Float u, Float v) {
  vertex[0] = x;
  vertex[2] = z;
  vertex[4] = 1.0f;
  vertex[6] = u;
  vertex[7] = v;
}
}  // namespace

auto GFX::MESHES::plane() -> GFX::MODEL::Scene {
  GFX::MODEL::Scene scene;
  GFX::MODEL::Mesh mesh;
  constexpr Whole POINTS = 4;
  const Word wound[6] = {0, 1, 2, 0, 2, 3};
  constexpr Whole SIZE = POINTS * STRIDE * sizeof(Float);
  mesh.vertices = GFX::BUFFERS::create(SIZE, GFX::BUFFERS::VERTICES);
  mesh.indices = GFX::BUFFERS::create(sizeof(wound), GFX::BUFFERS::INDICES);
  auto *floats = GFX::BUFFERS::GET::data<Float>(mesh.vertices);
  std::memset(floats, 0, SIZE);
  corner(floats, -HALF, -HALF, 0.0f, 0.0f);
  corner(floats + STRIDE, HALF, -HALF, 1.0f, 0.0f);
  corner(floats + 2 * STRIDE, HALF, HALF, 1.0f, 1.0f);
  corner(floats + 3 * STRIDE, -HALF, HALF, 0.0f, 1.0f);
  std::memcpy(
    GFX::BUFFERS::GET::data<Word>(mesh.indices), wound, sizeof(wound));
  mesh.count = 6;
  scene.meshes.push_back(mesh);
  GFX::MODEL::Node node;
  node.name = "Plane";
  GFX::MATH::reset(node.local);
  GFX::MATH::reset(node.world);
  node.count = 1;
  scene.nodes.push_back(node);
  scene.roots.push_back(0);
  scene.bounds.min[0] = scene.bounds.min[2] = -HALF;
  scene.bounds.max[0] = scene.bounds.max[2] = HALF;
  return scene;
}

auto GFX::MESHES::grid(Float extent, Whole steps) -> Vector<Float> {
  Vector<Float> vertices(steps * 4 * STRIDE, 0.0f);
  Float *floats = vertices.data();
  for (Whole line = 0; line < steps; line += 1) {
    const Float at = -extent + (2.0f * extent * line) / (steps - 1);
    auto *along = floats + line * 4 * STRIDE;
    corner(along, at, -extent, 0.0f, 0.0f);
    corner(along + STRIDE, at, extent, 0.0f, 0.0f);
    corner(along + 2 * STRIDE, -extent, at, 0.0f, 0.0f);
    corner(along + 3 * STRIDE, extent, at, 0.0f, 0.0f);
  }
  return vertices;
}
