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
#include <island/graphics/sprites/sprites.descriptor.internal.hpp>
#include <island/graphics/images/images.hpp>
#include <string>

namespace DESCRIPTOR = GFX::SPRITES::DESCRIPTOR;
namespace STATE = GFX::SPRITES::STATE;

static auto settle(const DESCRIPTOR::Cut &cut, const GFX::IMAGES::Image &image)
  -> GFX::SPRITES::ATLASES::Placement {
  auto placement = cut.placement;
  if (!cut.sourced) placement.source = placement.extent;
  const auto wide = static_cast<Float>(image.width);
  const auto tall = static_cast<Float>(image.height);
  placement.texture = {
    static_cast<Float>(cut.origin.x) / wide,
    static_cast<Float>(cut.origin.y) / tall,
    static_cast<Float>(placement.extent.width) / wide,
    static_cast<Float>(placement.extent.height) / tall};
  return placement;
}

static auto fits(
  const DESCRIPTOR::Cursor &cursor, const GFX::IMAGES::Image &image,
  const String &site) -> Status {
  for (Whole index = 0; index < cursor.cuts.size(); index += 1) {
    const auto &cut = cursor.cuts[index];
    if (
      cut.origin.x + cut.placement.extent.width <= image.width &&
      cut.origin.y + cut.placement.extent.height <= image.height)
      continue;
    return DESCRIPTOR::refuse(
      site, "frame '" + cursor.names[index] + "' reaches past the sheet");
  }
  return 0;
}

static auto read(DESCRIPTOR::Cursor &cursor, const String &path) -> Status {
  IO::STREAMS::Input file(path);
  if (!file) return DESCRIPTOR::refuse(path, "no such descriptor");
  Whole number = 0;
  for (String line; std::getline(file, line);) {
    number += 1;
    const String site = path + ":" + std::to_string(number);
    if (DESCRIPTOR::interpret(cursor, line, site) != 0) return 1;
  }
  if (cursor.image.empty())
    return DESCRIPTOR::refuse(path, "descriptor names no image");
  return 0;
}

auto GFX::SPRITES::descriptor(STRING::Hot path) -> Handle {
  const String site(path);
  DESCRIPTOR::Cursor cursor;
  if (::read(cursor, site) != 0) return NONE;
  const auto image = GFX::IMAGES::load(IO::sibling(site, cursor.image).c_str());
  if (image.rgba.empty()) return NONE;
  if (::fits(cursor, image, site) != 0) return NONE;
  const auto atlas =
    ATLASES::create(image.width, image.height, cursor.cell, image.rgba);
  for (Whole index = 0; index < cursor.cuts.size(); index += 1)
    STATE::atlases[atlas].frames.push_back(
      {cursor.names[index], ::settle(cursor.cuts[index], image)});
  return atlas;
}
