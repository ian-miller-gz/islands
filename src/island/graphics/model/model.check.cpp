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
#include <logger.hpp>
#include <cstring>
#define LOGGER_CATEGORY "~/island/graphics/model::"

static auto extra(const cgltf_material &material) -> Flag {
  return material.pbr_metallic_roughness.metallic_roughness_texture.texture ||
         material.normal_texture.texture ||
         material.occlusion_texture.texture ||
         material.emissive_texture.texture;
}

static auto reachable(const cgltf_material &material) -> Flag {
  const auto *sampled =
    material.pbr_metallic_roughness.base_color_texture.texture;
  if (!sampled) return true;
  const auto *image = sampled->image;
  if (!image) return false;
  return image->buffer_view ||
         (image->uri && strncmp(image->uri, "data:", 5) != 0);
}

static auto sparse(const cgltf_primitive &primitive) -> Flag {
  if (primitive.indices && primitive.indices->is_sparse) return true;
  for (cgltf_size i = 0; i < primitive.attributes_count; i += 1)
    if (primitive.attributes[i].data->is_sparse) return true;
  return false;
}

static auto known(const cgltf_attribute &attribute) -> Flag {
  return attribute.index == 0 &&
         (attribute.type == cgltf_attribute_type_position ||
          attribute.type == cgltf_attribute_type_normal ||
          attribute.type == cgltf_attribute_type_texcoord);
}

static void notice(const cgltf_primitive &primitive, const String &path) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "model");
  for (cgltf_size i = 0; i < primitive.attributes_count; i += 1)
    if (!known(primitive.attributes[i]))
      logger.warn(
        "Skipping model attribute in %s: %s.", path.c_str(),
        primitive.attributes[i].name);
  if (primitive.material && extra(*primitive.material))
    logger.warn(
      "Skipping model material textures in %s: beyond the base "
      "color.",
      path.c_str());
}

auto GFX::MODEL::supported(const cgltf_primitive &primitive, const String &path)
  -> Flag {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "model");
  STRING::Hot reason = nullptr;
  if (primitive.type != cgltf_primitive_type_triangles)
    reason = "primitive topology (only triangles import)";
  else if (!find(primitive, cgltf_attribute_type_position))
    reason = "primitive without a POSITION attribute";
  else if (sparse(primitive))
    reason = "sparse accessors";
  else if (primitive.material && !reachable(*primitive.material))
    reason = "base color images outside buffer views and plain file uris";
  if (reason) {
    logger.error("Unsupported model feature in %s: %s.", path.c_str(), reason);
    return false;
  }
  notice(primitive, path);
  return true;
}
