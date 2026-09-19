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
#include <island/graphics/images/images.hpp>
#include <logger.hpp>
#define LOGGER_CATEGORY "~/island/graphics/model::"

using Images = GFX::MODEL::Images;

static auto decode(const cgltf_image &image, const String &path)
  -> GFX::IMAGES::Image {
  if (image.buffer_view)
    return GFX::IMAGES::decode(
      reinterpret_cast<const Byte *>(cgltf_buffer_view_data(image.buffer_view)),
      static_cast<Whole>(image.buffer_view->size));
  String file = image.uri;
  cgltf_decode_uri(file.data());
  file = IO::sibling(path, file.c_str());
  return GFX::IMAGES::load(file.c_str());
}

static auto build(const cgltf_image &image, const String &path, Images &images)
  -> GFX::Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "model");
  if (const auto found = images.find(&image); found != images.end())
    return found->second;
  const auto decoded = decode(image, path);
  if (decoded.rgba.empty()) {
    logger.error("Cannot decode a model image in %s.", path.c_str());
    return GFX::NONE;
  }
  const auto texture = GFX::TEXTURES::create(decoded.width, decoded.height);
  GFX::TEXTURES::write(texture, decoded.rgba.data());
  images.emplace(&image, texture);
  return texture;
}

auto GFX::MODEL::texture(
  const cgltf_primitive &primitive, const String &path, Images &images,
  GFX::Handle &handle) -> Status {
  if (!primitive.material) return 0;
  const auto *sampled =
    primitive.material->pbr_metallic_roughness.base_color_texture.texture;
  if (!sampled) return 0;
  handle = build(*sampled->image, path, images);
  return handle == GFX::NONE;
}
