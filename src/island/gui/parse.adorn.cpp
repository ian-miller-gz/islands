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
#include <island/gui/parse.internal.hpp>

static auto paint(
  GUI::Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status {
  auto color = GFX::COLORS::WHITE;
  if (GUI::PARSE::read(value, color) != 0) return 1;
  if (key == "color") return GUI::set(document, id, color);
  if (key == "edge") return GUI::set(document, id, GUI::Edge{color});
  if (key == "hover") return GUI::set(document, id, GUI::Hover{color});
  if (key == "press") return GUI::set(document, id, GUI::Press{color});
  if (key == "glow") return GUI::set(document, id, GUI::Glow{color});
  return GUI::set(document, id, GUI::Wash{color});
}

static auto picture(
  GUI::Handle document, STRING::Hot id, const String &value,
  String::size_type colon) -> Status {
  GUI::Image image{value.substr(0, colon)};
  const auto cell = value.substr(colon + 1);
  const auto comma = cell.find(',');
  if (comma == String::npos) return 1;
  if (GUI::PARSE::read(cell.substr(0, comma), image.column) != 0) return 1;
  if (GUI::PARSE::read(cell.substr(comma + 1), image.row) != 0) return 1;
  return GUI::set(document, id, image);
}

auto GUI::PARSE::adorn(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status {
  if (key == "anchor") {
    auto anchor = GUI::NORTHWEST;
    if (read(value, anchor) == 0) return set(document, id, anchor);
    GUI::Stretch stretch;
    return read(value, stretch) != 0 ? 1 : set(document, id, stretch);
  }
  if (
    key == "color" || key == "edge" || key == "hover" || key == "press" ||
    key == "glow" || key == "wash")
    return paint(document, id, key, value);
  if (key == "frame") {
    const auto colon = value.find(':');
    if (colon != String::npos) return picture(document, id, value, colon);
    GFX::SPRITES::Frame frame;
    return read(value, frame) != 0 ? 1 : set(document, id, frame);
  }
  return 1;
}
