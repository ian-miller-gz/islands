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
// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/graphics/model/model.draw.hpp>
#include <island/graphics/math.hpp>

namespace {
void visit(
  const GFX::MODEL::Scene &scene, const GFX::MODEL::Node &node, GFX::Pass &pass,
  GFX::Handle pipeline, GFX::Handle sampler, const GFX::SHADERS::Camera &view) {
  GFX::SHADERS::Camera block;
  GFX::MATH::multiply(node.world, view.matrix, block.matrix);
  for (Whole m = node.first; m < node.first + node.count; m += 1) {
    const auto &mesh = scene.meshes[m];
    if (mesh.material.texture == GFX::NONE) continue;
    pass.draws.push_back(
      {.pipeline = pipeline,
       .uniforms = {GFX::pack(block)},
       .fragment = {.samplers = {{mesh.material.texture, sampler}}},
       .vertices = mesh.vertices,
       .indices = mesh.indices,
       .count = mesh.count});
  }
  for (const auto child : node.children)
    visit(scene, scene.nodes[child], pass, pipeline, sampler, view);
}
}  // namespace

void GFX::MODEL::draw(
  const Scene &scene, GFX::Pass &pass, GFX::Handle pipeline,
  GFX::Handle sampler, const GFX::SHADERS::Camera &camera) {
  for (const auto root : scene.roots)
    visit(scene, scene.nodes[root], pass, pipeline, sampler, camera);
}
