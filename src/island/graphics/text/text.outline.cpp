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
#include <island/graphics/text/text.outline.internal.hpp>
#include <island/graphics/backend/pipelines.hpp>
#include <cartridge.hpp>
#include <cstdlib>
#include <logger.hpp>
#define LOGGER_CATEGORY "~/island/graphics/text::"

namespace STATE = GFX::TEXT::OUTLINE::STATE;
namespace OUTLINE = GFX::TEXT::OUTLINE;

GFX::TEXT::OUTLINE::Face GFX::TEXT::OUTLINE::STATE::face;
GFX::Handle GFX::TEXT::OUTLINE::STATE::curves = GFX::NONE;
GFX::Handle GFX::TEXT::OUTLINE::STATE::strips = GFX::NONE;
GFX::Handle GFX::TEXT::OUTLINE::STATE::sampler = GFX::NONE;
GFX::Handle GFX::TEXT::OUTLINE::STATE::pipeline = GFX::NONE;
GFX::Handle GFX::TEXT::OUTLINE::STATE::buffer = GFX::NONE;
Whole GFX::TEXT::OUTLINE::STATE::cursor = 0;
Whole GFX::TEXT::OUTLINE::STATE::frame = 0;
Flag GFX::TEXT::OUTLINE::STATE::ready = false;
Flag GFX::TEXT::OUTLINE::STATE::refused = false;

auto GFX::TEXT::OUTLINE::tier() -> String {
  return CARTRIDGE::font == CARTRIDGE::FULL ? WIDE : SLIM;
}

static auto sheet(
  const String &name, Whole width, Whole height, GFX::TEXTURES::Format format,
  GFX::Handle &handle) -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "outline");
  const auto path = IO::sibling(OUTLINE::tier() + OUTLINE::SUFFIX, name);
  const auto bytes = IO::read(path);
  if (bytes.size() != width * height * GFX::TEXTURES::depth(format)) {
    logger.error(
      "Sheet extent disagrees with the directory: %s.", path.c_str());
    return 1;
  }
  handle = GFX::TEXTURES::create(width, height, format);
  GFX::TEXTURES::write(handle, bytes.data());
  return 0;
}

auto GFX::TEXT::OUTLINE::upload() -> Status {
  return ::sheet(
           STATE::face.sheets[0], STATE::face.curves[0], STATE::face.curves[1],
           GFX::TEXTURES::RGBA16F, STATE::curves) ||
         ::sheet(
           STATE::face.sheets[1], STATE::face.strips[0], STATE::face.strips[1],
           GFX::TEXTURES::RG32F, STATE::strips);
}

static void build() {
  STATE::sampler = GFX::SAMPLERS::create(GFX::SAMPLERS::CLAMP);
  STATE::pipeline = GFX::PIPELINES::create(
    {.vertex =
       {.path = String(OUTLINE::SHADERS) + ".vertex",
        .uniforms = {sizeof(OUTLINE::Camera)}},
     .fragment =
       {.path = String(OUTLINE::SHADERS) + ".fragment", .samplers = 2},
     .attributes = {4, 4, 4, 4, 4},
     .depth = true});
  STATE::buffer = GFX::BUFFERS::create(
    OUTLINE::LIMIT * OUTLINE::CORNERS * OUTLINE::WIDTHS * sizeof(Float),
    GFX::BUFFERS::VERTICES);
}

auto GFX::TEXT::OUTLINE::ready() -> Flag {
  if (STATE::ready) return true;
  if (STATE::refused) return false;
  if (parse(OUTLINE::tier() + SUFFIX, STATE::face) || upload()) {
    STATE::refused = true;
    return false;
  }
  ::build();
  STATE::ready = true;
  return true;
}

void GFX::TEXT::OUTLINE::close() {
  STATE::face = {};
  STATE::curves = STATE::strips = GFX::NONE;
  STATE::sampler = STATE::pipeline = STATE::buffer = GFX::NONE;
  STATE::cursor = 0;
  STATE::frame = 0;
  STATE::ready = false;
  STATE::refused = false;
}
