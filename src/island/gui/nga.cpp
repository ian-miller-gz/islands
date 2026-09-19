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
#include <island/gui/nga.internal.hpp>

auto GUI::NGA::set(Handle document, STRING::Hot id, Port port) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  const auto side = static_cast<Whole>(port.side);
  if (
    properties.port == side && properties.kind == port.kind &&
    properties.mute == port.mute)
    return 0;
  properties.port = side;
  properties.kind = port.kind;
  properties.mute = port.mute;
  return mark(document, node);
}

auto GUI::NGA::set(Handle document, STRING::Hot id, Pan pan) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  const auto settled = bounded(document, node, pan);
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.pan.x == settled.x && properties.pan.y == settled.y) return 0;
  properties.pan = {settled.x, settled.y};
  return mark(document, node);
}

auto GUI::NGA::resolved(Zoom zoom) -> Extent {
  return {zoom.value, zoom.down == Zoom::SAME ? zoom.value : zoom.down};
}

auto GUI::NGA::set(Handle document, STRING::Hot id, Zoom zoom) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  const auto scale = resolved(zoom);
  if (scale.w <= 0.0f || scale.h <= 0.0f) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.zoom == scale) return 0;
  properties.zoom = scale;
  const auto settled =
    bounded(document, node, {properties.pan.x, properties.pan.y});
  properties.pan = {settled.x, settled.y};
  return mark(document, node);
}

auto GUI::NGA::set(Handle document, STRING::Hot id, Pinned pinned) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  const auto axis = static_cast<Whole>(pinned.axis);
  if (properties.pinned == axis) return 0;
  properties.pinned = axis;
  return mark(document, node);
}

auto GUI::NGA::GET::port(Handle document, STRING::Hot id) -> Port {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const auto &properties = STATE::documents[document].nodes[node].properties;
  const auto side = properties.port;
  if (side > Port::OUT) return {};
  return {static_cast<Port::Side>(side), properties.kind, properties.mute};
}

auto GUI::NGA::GET::pan(Handle document, STRING::Hot id) -> Pan {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const auto &properties = STATE::documents[document].nodes[node].properties;
  return {properties.pan.x, properties.pan.y};
}

auto GUI::NGA::GET::zoom(Handle document, STRING::Hot id) -> Zoom {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {1.0f, 1.0f};
  const auto scale = STATE::documents[document].nodes[node].properties.zoom;
  return {scale.w, scale.h};
}

auto GUI::NGA::GET::pinned(Handle document, STRING::Hot id) -> Pinned {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const auto axis = STATE::documents[document].nodes[node].properties.pinned;
  if (axis > static_cast<Whole>(Pinned::DOWN)) return {};
  return {static_cast<Pinned::Axis>(axis)};
}
