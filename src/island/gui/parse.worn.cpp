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
#include <island/gui/gui.internal.hpp>
#include <island/gui/parse.internal.hpp>

static auto gauge(
  GUI::Properties &properties, const String &key,
  const String &value) -> Status {
  Float number = 0.0f;
  if (GUI::PARSE::read(value, number) != 0) return 1;
  if (key == "x") properties.position.x = number;
  if (key == "y") properties.position.y = number;
  if (key == "w") properties.extent.w = number;
  if (key == "h") properties.extent.h = number;
  if (key == "fraction") properties.fraction = number;
  if (key == "value") properties.value = number;
  if (key == "least") properties.least = number;
  if (key == "most") properties.most = number;
  if (key == "resting") properties.resting = number;
  if (key == "z") properties.z = number;
  if (key == "border") properties.border = number;
  if (key == "pad") properties.pad = number;
  if (key == "size") properties.size = number;
  return 0;
}

static auto paint(
  GUI::Properties &properties, const String &key,
  const String &value) -> Status {
  auto color = GFX::COLORS::WHITE;
  if (GUI::PARSE::read(value, color) != 0) return 1;
  if (key == "color") properties.color = color;
  if (key == "edge") properties.edge = color;
  if (key == "hover") properties.hover = color;
  if (key == "press") properties.press = color;
  if (key == "glow") properties.glow = color;
  if (key == "wash") properties.wash = color;
  return 0;
}

static auto picture(
  GUI::Properties &properties, const String &value,
  String::size_type colon) -> Status {
  const String name = value.substr(0, colon);
  const auto found = GUI::STATE::bindings.find(name);
  if (found == GUI::STATE::bindings.end()) return 1;
  const auto cell = value.substr(colon + 1);
  const auto comma = cell.find(',');
  if (comma == String::npos) return 1;
  Whole column = 0, row = 0;
  if (GUI::PARSE::read(cell.substr(0, comma), column) != 0) return 1;
  if (GUI::PARSE::read(cell.substr(comma + 1), row) != 0) return 1;
  properties.atlas = found->second;
  properties.atlas_name = name;
  properties.frame = GFX::SPRITES::ATLASES::frame(found->second, column, row);
  return 0;
}

static auto count(
  GUI::Properties &properties, const String &key,
  const String &value) -> Status {
  Whole number = 0;
  if (GUI::PARSE::read(value, number) != 0) return 1;
  if (key == "rows") properties.rows = number;
  if (key == "cursor") properties.cursor = number;
  if (key == "first") properties.first = number;
  if (key == "steps") properties.steps = number;
  return 0;
}

auto GUI::PARSE::apply(
  Properties &properties, const String &key, const String &value) -> Status {
  if (
    key == "x" || key == "y" || key == "w" || key == "h" || key == "fraction" ||
    key == "z" || key == "border" || key == "pad" || key == "size" ||
    key == "value" || key == "least" || key == "most" || key == "resting")
    return ::gauge(properties, key, value);
  if (key == "rows" || key == "cursor" || key == "first" || key == "steps")
    return ::count(properties, key, value);
  if (key == "pitch") {
    Float number = 0.0f;
    if (read(value, number) != 0) return 1;
    properties.pitch = number;
    return 0;
  }
  if (key == "mark") {
    properties.mark = value;
    return 0;
  }
  if (key == "plain") {
    properties.plain = value;
    return 0;
  }
  if (key == "text") {
    properties.text = value;
    return 0;
  }
  if (key == "wrap") {
    Whole columns = 0;
    if (read(value, columns) != 0) return 1;
    properties.wrap = columns;
    return 0;
  }
  if (
    key == "visible" || key == "open" || key == "clip" || key == "quick" ||
    key == "scroll" || key == "graphic") {
    Flag flag = false;
    if (read(value, flag) != 0) return 1;
    (key == "visible"  ? properties.visible
     : key == "open"   ? properties.open
     : key == "clip"   ? properties.clip
     : key == "quick"  ? properties.quick
     : key == "scroll" ? properties.scroll
                       : properties.graphic) = flag;
    return 0;
  }
  if (key == "axis") {
    Axis axis;
    if (read(value, axis) != 0) return 1;
    properties.across = axis.across;
    properties.free = axis.free;
    return 0;
  }
  if (key == "carry") {
    Carry carry;
    if (read(value, carry) != 0) return 1;
    properties.carried = carry.pointer;
    return 0;
  }
  if (seated(key)) return seat(properties, key, value);
  if (stroked(key)) return stroke(properties, key, value);
  if (boarded(key)) return board(properties, key, value);
  return adorn(properties, key, value);
}

auto GUI::PARSE::adorn(
  Properties &properties, const String &key, const String &value) -> Status {
  if (key == "anchor") {
    if (read(value, properties.anchor) == 0) return 0;
    Stretch stretch;
    if (read(value, stretch) != 0) return 1;
    properties.pins = stretch.edges;
    return 0;
  }
  if (
    key == "color" || key == "edge" || key == "hover" || key == "press" ||
    key == "glow" || key == "wash")
    return ::paint(properties, key, value);
  if (key == "frame") {
    const auto colon = value.find(':');
    if (colon != String::npos) return ::picture(properties, value, colon);
    GFX::SPRITES::Frame frame;
    if (read(value, frame) != 0) return 1;
    properties.frame = frame;
    return 0;
  }
  return 1;
}
