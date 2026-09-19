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
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA
#include <stb/stb_image.h>

#include <island/graphics/images/images.hpp>
#include <logger.hpp>
#include <algorithm>
#define LOGGER_CATEGORY "~/island/graphics/images::"

static constexpr STRING::Hot EXTENSIONS[] = {
  "png", "bmp", "tga", "jpg", "jpeg"};

static auto extension(const String &path) -> String {
  const auto dot = path.find_last_of('.');
  if (dot == String::npos) return "";
  String name = path.substr(dot + 1);
  std::transform(name.begin(), name.end(), name.begin(), [](char letter) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(letter)));
  });
  return name;
}

static auto declared(const String &path) -> Flag {
  const String name = ::extension(path);
  for (const auto *known : EXTENSIONS)
    if (name == known) return true;
  return false;
}

static auto adopt(stbi_uc *pixels, int width, int height)
  -> GFX::IMAGES::Image {
  GFX::IMAGES::Image image;
  if (!pixels) return image;
  image.width = static_cast<Whole>(width);
  image.height = static_cast<Whole>(height);
  const auto *bytes = reinterpret_cast<const Byte *>(pixels);
  image.rgba.assign(
    bytes, bytes + image.width * image.height * GFX::TEXTURES::STRIDE);
  stbi_image_free(pixels);
  return image;
}

auto GFX::IMAGES::load(STRING::Hot path) -> Image {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "images");
  constexpr auto DEPTH = static_cast<int>(GFX::TEXTURES::STRIDE);
  if (!::declared(path)) {
    logger.error(
      String("Cannot decode ") + path + ": the decoded formats are " + FORMATS +
      ".");
    return {};
  }
  int width = 0, height = 0, channels = 0;
  auto *pixels = stbi_load(path, &width, &height, &channels, DEPTH);
  if (!pixels) {
    logger.error(
      String("Cannot decode ") + path + ": " + stbi_failure_reason() + ".");
    return {};
  }
  return ::adopt(pixels, width, height);
}

auto GFX::IMAGES::decode(const Byte *bytes, Whole length) -> Image {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "images");
  constexpr auto DEPTH = static_cast<int>(GFX::TEXTURES::STRIDE);
  int width = 0, height = 0, channels = 0;
  auto *pixels = stbi_load_from_memory(
    reinterpret_cast<const stbi_uc *>(bytes), static_cast<int>(length), &width,
    &height, &channels, DEPTH);
  if (!pixels) {
    logger.error(
      String("Cannot decode the embedded image: ") + stbi_failure_reason() +
      ".");
    return {};
  }
  return ::adopt(pixels, width, height);
}
