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
#include <island/graphics/model/model.internal.hpp>

static auto offsets(const cgltf_data *data) -> Vector<Whole> {
  Vector<Whole> firsts;
  Whole first = 0;
  for (cgltf_size m = 0; m < data->meshes_count; m += 1) {
    firsts.push_back(first);
    first += data->meshes[m].primitives_count;
  }
  return firsts;
}

static void roots(const cgltf_data *data, Vector<Whole> &indices) {
  const auto *scene = data->scene               ? data->scene
                      : data->scenes_count != 0 ? &data->scenes[0]
                                                : nullptr;
  if (scene) {
    for (cgltf_size n = 0; n < scene->nodes_count; n += 1)
      indices.push_back(scene->nodes[n] - data->nodes);
    return;
  }
  for (cgltf_size n = 0; n < data->nodes_count; n += 1)
    if (!data->nodes[n].parent) indices.push_back(n);
}

void GFX::MODEL::compose(const cgltf_data *data, Scene &scene) {
  const auto firsts = offsets(data);
  for (cgltf_size n = 0; n < data->nodes_count; n += 1) {
    const auto &node = data->nodes[n];
    auto &entry = scene.nodes.emplace_back();
    if (node.name) entry.name = node.name;
    cgltf_node_transform_local(&node, entry.local);
    cgltf_node_transform_world(&node, entry.world);
    if (node.mesh) {
      entry.first = firsts[node.mesh - data->meshes];
      entry.count = node.mesh->primitives_count;
    }
    for (cgltf_size c = 0; c < node.children_count; c += 1)
      entry.children.push_back(node.children[c] - data->nodes);
  }
  roots(data, scene.roots);
}
