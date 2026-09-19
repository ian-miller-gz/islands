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
#include <island/graphics/meshes/meshes.hpp>

namespace {
const Whole WINDING[GFX::MESHES::VERTICES][2] = {{0, 0}, {1, 0}, {0, 1},
                                                 {1, 0}, {1, 1}, {0, 1}};
}  // namespace

auto GFX::MESHES::quad(Float x, Float y, Float w, Float h, GFX::Color color)
  -> Vector<Float> {
  Vector<Float> vertices;
  vertices.reserve(VERTICES * COLORED);
  for (const auto &corner : WINDING) {
    vertices.push_back(x + corner[0] * w);
    vertices.push_back(y + corner[1] * h);
    vertices.push_back(color.r);
    vertices.push_back(color.g);
    vertices.push_back(color.b);
    vertices.push_back(color.a);
  }
  return vertices;
}

auto GFX::MESHES::quad(Float x, Float y, Float w, Float h) -> Vector<Float> {
  Vector<Float> vertices;
  vertices.reserve(VERTICES * TEXTURED);
  for (const auto &corner : WINDING) {
    vertices.push_back(x + corner[0] * w);
    vertices.push_back(y + corner[1] * h);
    vertices.push_back(static_cast<Float>(corner[0]));
    vertices.push_back(static_cast<Float>(corner[1]));
  }
  return vertices;
}

auto GFX::MESHES::fullscreen() -> Vector<Float> {
  Vector<Float> vertices;
  vertices.reserve(VERTICES * TEXTURED);
  for (const auto &corner : WINDING) {
    vertices.push_back(-1.0f + corner[0] * 2.0f);
    vertices.push_back(1.0f - corner[1] * 2.0f);
    vertices.push_back(static_cast<Float>(corner[0]));
    vertices.push_back(static_cast<Float>(corner[1]));
  }
  return vertices;
}
