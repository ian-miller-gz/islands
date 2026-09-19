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

auto GUI::GET::position(Handle document, STRING::Hot id) -> Position {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.position;
}

auto GUI::GET::extent(Handle document, STRING::Hot id) -> Extent {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.extent;
}

auto GUI::GET::color(Handle document, STRING::Hot id) -> GFX::Color {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.color;
}

auto GUI::GET::text(Handle document, STRING::Hot id) -> STRING::Cold {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.text;
}

auto GUI::GET::fraction(Handle document, STRING::Hot id) -> Float {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0.0f;
  return STATE::documents[document].nodes[node].properties.fraction;
}

auto GUI::GET::visibility(Handle document, STRING::Hot id) -> Flag {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return false;
  return STATE::documents[document].nodes[node].properties.visible;
}

auto GUI::GET::anchor(Handle document, STRING::Hot id) -> Anchor {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return NORTHWEST;
  return STATE::documents[document].nodes[node].properties.anchor;
}

auto GUI::GET::stretch(Handle document, STRING::Hot id) -> Whole {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0;
  return STATE::documents[document].nodes[node].properties.pins;
}

auto GUI::GET::frame(Handle document, STRING::Hot id) -> GFX::SPRITES::Frame {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.frame;
}

auto GUI::GET::depth(Handle document, STRING::Hot id) -> Float {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0.0f;
  return STATE::documents[document].nodes[node].properties.z;
}

auto GUI::GET::dirty(Handle document) -> Flag {
  if (!live(document)) return false;
  return STATE::documents[document].dirty;
}

auto GUI::GET::dirty(Handle document, STRING::Hot id) -> Flag {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return false;
  return STATE::documents[document].nodes[node].dirty;
}

auto GUI::GET::restructured(Handle document) -> Flag {
  if (!live(document)) return false;
  return STATE::documents[document].restructured;
}

auto GUI::GET::generation(Handle document) -> Whole {
  if (!live(document)) return UNBORN;
  return STATE::documents[document].generation;
}
