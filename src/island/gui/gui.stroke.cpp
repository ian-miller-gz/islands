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

auto GUI::set(Handle document, STRING::Hot id, Ends ends) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.to == ends.to) return 0;
  properties.to = ends.to;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Shape shape) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  const auto run = static_cast<Whole>(shape.run);
  if (properties.shape == run) return 0;
  properties.shape = run;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Bend bend) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.bend == bend.value) return 0;
  properties.bend = bend.value;
  return mark(document, node);
}

auto GUI::GET::ends(Handle document, STRING::Hot id) -> Ends {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return {STATE::documents[document].nodes[node].properties.to};
}

auto GUI::GET::shape(Handle document, STRING::Hot id) -> Shape {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const auto &properties = STATE::documents[document].nodes[node].properties;
  return {static_cast<Shape::Run>(properties.shape)};
}

auto GUI::GET::bend(Handle document, STRING::Hot id) -> Float {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.bend;
}
