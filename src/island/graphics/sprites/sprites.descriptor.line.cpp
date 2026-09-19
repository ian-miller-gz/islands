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
#include <logger.hpp>
#include <sstream>
#define LOGGER_CATEGORY "~/island/graphics/sprites::"

namespace DESCRIPTOR = GFX::SPRITES::DESCRIPTOR;

auto DESCRIPTOR::refuse(const String &site, const String &reason) -> Status {
  LOGGER::get(LOGGER_CATEGORY "atlases").error(site + ": " + reason + ".");
  return 1;
}

static auto declare(
  DESCRIPTOR::Cursor &cursor, std::istringstream &fields, const String &site) {
  String name;
  if (!(fields >> name))
    return DESCRIPTOR::refuse(site, "frame without a name");
  for (const String &taken : cursor.names)
    if (taken == name)
      return DESCRIPTOR::refuse(site, "frame '" + name + "' declared twice");
  DESCRIPTOR::Cut cut;
  for (String field; fields >> field;) {
    const auto split = field.find('=');
    if (split == String::npos)
      return DESCRIPTOR::refuse(site, "field without a value '" + field + "'");
    const String key = field.substr(0, split);
    const Status answer = DESCRIPTOR::apply(key, field.substr(split + 1), cut);
    if (answer == DESCRIPTOR::UNKNOWN)
      return DESCRIPTOR::refuse(site, "unknown frame field '" + key + "'");
    if (answer != 0)
      return DESCRIPTOR::refuse(site, "field '" + field + "' does not read");
  }
  if (!cut.placement.extent.width || !cut.placement.extent.height)
    return DESCRIPTOR::refuse(site, "frame '" + name + "' has no w and h");
  cursor.names.push_back(name);
  cursor.cuts.push_back(cut);
  return Status(0);
}

static auto measure(
  DESCRIPTOR::Cursor &cursor, std::istringstream &fields, const String &site) {
  String width;
  String height;
  if (!(fields >> width) || !(fields >> height))
    return DESCRIPTOR::refuse(site, "cell wants a width and a height");
  if (
    DESCRIPTOR::number(width, cursor.cell.width) != 0 ||
    DESCRIPTOR::number(height, cursor.cell.height) != 0)
    return DESCRIPTOR::refuse(site, "cell extent does not read");
  return Status(0);
}

auto DESCRIPTOR::interpret(
  Cursor &cursor, const String &line, const String &site) -> Status {
  std::istringstream fields(line.substr(0, line.find('#')));
  String keyword;
  if (!(fields >> keyword)) return 0;
  if (keyword == "frame") return ::declare(cursor, fields, site);
  if (keyword == "cell") return ::measure(cursor, fields, site);
  if (keyword != "image")
    return refuse(site, "unknown keyword '" + keyword + "'");
  if (!(fields >> cursor.image)) return refuse(site, "image without a file");
  return 0;
}
