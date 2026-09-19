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
#include <island.hpp>
#include <island/graphics/backend/vulkan/resources.internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#include <utility>

#if SR_GFX_BACKEND == SR_VULKAN

struct Image {
  GFX::Texture texture;
  Whole width = 0;
  Whole height = 0;
  Whole depth = GFX::TEXTURES::STRIDE;
  Flag storage = false;
  Flag filled = false;
};

static Vector<Image> images;

static auto translate(GFX::TEXTURES::Format format) -> vk::Format {
  if (format == GFX::TEXTURES::RGBA16F) return vk::Format::eR16G16B16A16Sfloat;
  if (format == GFX::TEXTURES::RG32F) return vk::Format::eR32G32Sfloat;
  return vk::Format::eR8G8B8A8Unorm;
}

auto GFX::TEXTURES::create(Whole width, Whole height, Format format)
  -> GFX::Handle {
  images.push_back(
    {GFX::BACKEND::VULKAN::IMAGE::create(width, height, ::translate(format)),
     width, height, depth(format)});
  return images.size() - 1;
}

void GFX::TEXTURES::write(GFX::Handle texture, const void *pixels) {
  auto &image = images[texture];
  GFX::BACKEND::VULKAN::upload(
    image.texture, pixels, image.width, image.height, image.depth);
  image.filled = true;
}

void GFX::TEXTURES::write(
  GFX::Handle texture, const void *pixels, Whole x, Whole y, Whole width,
  Whole height) {
  auto &image = images[texture];
  GFX::BACKEND::VULKAN::PATCHES::stage(
    texture, pixels, x, y, width, height, image.depth, !image.filled);
  image.filled = true;
}

void GFX::TEXTURES::destroy(GFX::Handle texture) {
  auto &image = images[texture];
  GFX::BACKEND::VULKAN::RETIRED::file(std::move(image.texture));
  image.texture = {};
  image.filled = false;
}

void GFX::TEXTURES::close() {
  GFX::BACKEND::VULKAN::PATCHES::close();
  for (auto &image : images) image.texture.close();
  images.clear();
}

auto GFX::BACKEND::VULKAN::GET::texture(GFX::Handle handle) -> GFX::Texture & {
  return images[handle].texture;
}

auto GFX::BACKEND::VULKAN::GET::storage(GFX::Handle handle) -> Flag {
  return images[handle].storage;
}

auto GFX::BACKEND::VULKAN::adopt(
  GFX::Texture texture, Whole width, Whole height,
  Flag storage) -> GFX::Handle {
  images.push_back({std::move(texture), width, height, storage});
  return images.size() - 1;
}

#endif
