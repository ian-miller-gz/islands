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
#include <island/graphics/model/model.hpp>
#include <island/graphics/model/model.internal.hpp>
#include <logger.hpp>
#define LOGGER_CATEGORY "~/island/graphics/model::"

static auto parse(const String &path, cgltf_data *&data) -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "model");
  cgltf_options options = {};
  cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);
  if (result != cgltf_result_success) {
    logger.error("Cannot parse model: %s (result %d).", path.c_str(), result);
    return 1;
  }
  result = cgltf_load_buffers(&options, data, path.c_str());
  if (result != cgltf_result_success) {
    logger.error(
      "Cannot load model buffers: %s (result %d).", path.c_str(), result);
    return 1;
  }
  result = cgltf_validate(data);
  if (result != cgltf_result_success) {
    logger.error("Invalid model: %s (result %d).", path.c_str(), result);
    return 1;
  }
  return 0;
}

auto GFX::MODEL::load(
  const cgltf_primitive &primitive, const String &path, Images &images,
  Scene &scene) -> Status {
  if (!supported(primitive, path)) return 1;
  scene.meshes.push_back(import(primitive));
  return texture(primitive, path, images, scene.meshes.back().material.texture);
}

auto GFX::MODEL::load(const cgltf_data &data, const String &path, Scene &scene)
  -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "model");
  Images images;
  for (cgltf_size m = 0; m < data.meshes_count; m += 1)
    for (cgltf_size p = 0; p < data.meshes[m].primitives_count; p += 1)
      if (load(data.meshes[m].primitives[p], path, images, scene)) {
        destroy(scene);
        return 1;
      }
  compose(&data, scene);
  measure(&data, scene);
  logger.debug(
    "Model imported: %s (%zu meshes, %zu nodes).", path.c_str(),
    scene.meshes.size(), scene.nodes.size());
  return 0;
}

auto GFX::MODEL::load(const String &path, Scene &scene) -> Status {
  destroy(scene);
  cgltf_data *data = nullptr;
  Status status = parse(path, data);
  if (!status) status = load(*data, path, scene);
  cgltf_free(data);
  return status;
}
