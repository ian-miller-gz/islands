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

auto GUI::set(Handle document, STRING::Hot id, Position position) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.position == position) return 0;
  properties.position = position;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Extent extent) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.extent == extent) return 0;
  properties.extent = extent;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, GFX::Color color) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.color == color) return 0;
  properties.color = color;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Text text) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.text == text.value) return 0;
  properties.text = text.value;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Fraction fraction) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.fraction == fraction.value) return 0;
  properties.fraction = fraction.value;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Visibility visibility)
  -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.visible == visibility.visible) return 0;
  properties.visible = visibility.visible;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Anchor anchor) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.anchor == anchor) return 0;
  properties.anchor = anchor;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Stretch stretch) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.pins == stretch.edges) return 0;
  properties.pins = stretch.edges;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, GFX::SPRITES::Frame frame)
  -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.frame == frame) return 0;
  properties.frame = frame;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Depth depth) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.z == depth.value) return 0;
  properties.z = depth.value;
  return mark(document, node);
}
