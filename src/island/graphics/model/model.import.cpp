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
#include <algorithm>
#include <iterator>

auto GFX::MODEL::find(
  const cgltf_primitive &primitive,
  cgltf_attribute_type type) -> const cgltf_accessor * {
  for (cgltf_size i = 0; i < primitive.attributes_count; i += 1)
    if (
      primitive.attributes[i].type == type &&
      primitive.attributes[i].index == 0)
      return primitive.attributes[i].data;
  return nullptr;
}

static void interleave(const cgltf_primitive &primitive, Float *vertices) {
  const cgltf_accessor *slots[] = {
    GFX::MODEL::find(primitive, cgltf_attribute_type_position),
    GFX::MODEL::find(primitive, cgltf_attribute_type_normal),
    GFX::MODEL::find(primitive, cgltf_attribute_type_texcoord)};
  constexpr Whole WIDTHS[] = {
    GFX::MODEL::LAYOUT::POSITION, GFX::MODEL::LAYOUT::NORMAL,
    GFX::MODEL::LAYOUT::TEXCOORD};
  for (cgltf_size vertex = 0; vertex < slots[0]->count; vertex += 1)
    for (Whole slot = 0; slot < std::size(slots); slot += 1) {
      std::fill_n(vertices, WIDTHS[slot], 0.0f);
      if (slots[slot])
        cgltf_accessor_read_float(slots[slot], vertex, vertices, WIDTHS[slot]);
      vertices += WIDTHS[slot];
    }
}

auto GFX::MODEL::import(const cgltf_primitive &primitive) -> Mesh {
  Mesh mesh;
  const auto count = find(primitive, cgltf_attribute_type_position)->count;
  mesh.vertices = GFX::BUFFERS::create(
    count * LAYOUT::STRIDE * sizeof(Float), GFX::BUFFERS::VERTICES);
  interleave(primitive, GFX::BUFFERS::GET::data<Float>(mesh.vertices));
  mesh.count =
    static_cast<Whole>(primitive.indices ? primitive.indices->count : count);
  if (primitive.indices) {
    mesh.indices =
      GFX::BUFFERS::create(mesh.count * sizeof(Word), GFX::BUFFERS::INDICES);
    auto *order = GFX::BUFFERS::GET::data<Word>(mesh.indices);
    for (cgltf_size index = 0; index < primitive.indices->count; index += 1)
      order[index] =
        static_cast<Word>(cgltf_accessor_read_index(primitive.indices, index));
  }
  if (primitive.material && primitive.material->has_pbr_metallic_roughness) {
    const auto &pbr = primitive.material->pbr_metallic_roughness;
    const auto *factor = pbr.base_color_factor;
    mesh.material.color = {factor[0], factor[1], factor[2], factor[3]};
    mesh.material.metallic = pbr.metallic_factor;
    mesh.material.roughness = pbr.roughness_factor;
  }
  return mesh;
}

void GFX::MODEL::destroy(Scene &scene) {
  Vector<GFX::Handle> textures;
  for (const auto &mesh : scene.meshes) {
    if (mesh.vertices != GFX::NONE) GFX::BUFFERS::destroy(mesh.vertices);
    if (mesh.indices != GFX::NONE) GFX::BUFFERS::destroy(mesh.indices);
    if (
      mesh.material.texture != GFX::NONE &&
      std::find(textures.begin(), textures.end(), mesh.material.texture) ==
        textures.end())
      textures.push_back(mesh.material.texture);
  }
  for (const auto texture : textures) GFX::TEXTURES::destroy(texture);
  scene.meshes.clear();
  scene.nodes.clear();
  scene.roots.clear();
  scene.bounds = {};
}
