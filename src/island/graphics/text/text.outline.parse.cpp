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
#include <cstdlib>
#include <logger.hpp>
#define LOGGER_CATEGORY "~/island/graphics/text::"

static auto value(const String &line, const String &key) -> String {
  const auto mark = line.find(" " + key + "=");
  if (mark == String::npos) return "";
  auto start = mark + key.size() + 2;
  if (start < line.size() && line[start] == '"') {
    start += 1;
    return line.substr(start, line.find('"', start) - start);
  }
  return line.substr(start, line.find(' ', start) - start);
}

static auto number(const String &line, const String &key) -> Float {
  return std::strtof(value(line, key).c_str(), nullptr);
}

static auto count(const String &line, const String &key) -> Whole {
  return static_cast<Whole>(
    std::strtoul(value(line, key).c_str(), nullptr, 10));
}

static void record(const String &line, GFX::TEXT::OUTLINE::Face &face) {
  const GFX::TEXT::OUTLINE::Glyph entry = {
    .data = {count(line, "x"), count(line, "y")},
    .bands = {count(line, "xbands"), count(line, "ybands")},
    .low = {number(line, "xmin"), number(line, "ymin")},
    .high = {number(line, "xmax"), number(line, "ymax")},
    .advance = number(line, "advance")};
  face.glyphs.emplace(count(line, "id"), entry);
}

static void assign(const String &line, GFX::TEXT::OUTLINE::Face &face) {
  const auto tag = line.substr(0, line.find(' '));
  if (tag == "outline") {
    face.line = number(line, "line");
    face.base = number(line, "base");
  }
  if (tag == "curves" || tag == "bands") {
    const Whole side = tag == "bands";
    (side ? face.strips : face.curves)[0] = count(line, "width");
    (side ? face.strips : face.curves)[1] = count(line, "height");
    face.sheets[side] = value(line, "file");
  }
  if (tag == "glyph") record(line, face);
  if (tag == "kerning")
    face.kernings.emplace(
      GFX::TEXT::pair(count(line, "first"), count(line, "second")),
      number(line, "amount"));
}

auto GFX::TEXT::OUTLINE::parse(const String &path, Face &face) -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "outline");
  IO::STREAMS::Input file(path);
  if (!file) {
    logger.error("Cannot read the outline directory: " + path + ".");
    return 1;
  }
  String line;
  while (std::getline(file, line)) ::assign(line, face);
  if (
    !face.glyphs.empty() && !face.sheets[0].empty() && !face.sheets[1].empty())
    return 0;
  logger.error("Outline directory names no glyphs or sheets: " + path + ".");
  return 1;
}
