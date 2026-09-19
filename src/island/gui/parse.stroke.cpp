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

namespace {

auto ended(const String &value, GUI::Ends &ends) -> Status {
  const auto comma = value.find(',');
  if (comma == String::npos) return 1;
  GUI::Ends read;
  if (GUI::PARSE::read(value.substr(0, comma), read.to.x) != 0) return 1;
  if (GUI::PARSE::read(value.substr(comma + 1), read.to.y) != 0) return 1;
  ends = read;
  return 0;
}

}  // namespace

auto GUI::PARSE::stroked(const String &key) -> Flag {
  return key == "to" || key == "shape" || key == "bend";
}

auto GUI::PARSE::stroke(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status {
  if (key == "to") {
    Ends ends;
    return ::ended(value, ends) != 0 ? 1 : set(document, id, ends);
  }
  if (key == "shape") {
    Shape shape;
    return read(value, shape) != 0 ? 1 : set(document, id, shape);
  }
  Bend bend;
  return read(value, bend.value) != 0 ? 1 : set(document, id, bend);
}

auto GUI::PARSE::stroke(
  Properties &properties, const String &key, const String &value) -> Status {
  if (key == "to") {
    Ends ends;
    if (::ended(value, ends) != 0) return 1;
    properties.to = ends.to;
    return 0;
  }
  if (key == "shape") {
    Shape shape;
    if (read(value, shape) != 0) return 1;
    properties.shape = static_cast<Whole>(shape.run);
    return 0;
  }
  Float bend = 0.0f;
  if (read(value, bend) != 0) return 1;
  properties.bend = bend;
  return 0;
}
