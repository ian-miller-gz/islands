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

auto GUI::set(Handle document, STRING::Hot id, Border border) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.border == border.width) return 0;
  properties.border = border.width;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Edge edge) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.edge == edge.color) return 0;
  properties.edge = edge.color;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Hover hover) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.hover == hover.color) return 0;
  properties.hover = hover.color;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Press press) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.press == press.color) return 0;
  properties.press = press.color;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Glow glow) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.glow == glow.color) return 0;
  properties.glow = glow.color;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Wash wash) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.wash == wash.color) return 0;
  properties.wash = wash.color;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Pad pad) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.pad == pad.value) return 0;
  properties.pad = pad.value;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Size size) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.size == size.value) return 0;
  properties.size = size.value;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Wrap wrap) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.wrap == wrap.columns) return 0;
  properties.wrap = wrap.columns;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Opening opening) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.open == opening.open) return 0;
  properties.open = opening.open;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Clipping clipping) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.clip == clipping.clip) return 0;
  properties.clip = clipping.clip;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Image image) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  const auto found = STATE::bindings.find(image.atlas);
  if (found == STATE::bindings.end()) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  const GFX::SPRITES::Frame frame =
    GFX::SPRITES::ATLASES::frame(found->second, image.column, image.row);
  if (
    properties.atlas == found->second && properties.atlas_name == image.atlas &&
    properties.frame == frame)
    return 0;
  properties.atlas = found->second;
  properties.atlas_name = image.atlas;
  properties.frame = frame;
  return mark(document, node);
}
