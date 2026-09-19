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
#include <island/gui/backend.hpp>

auto GUI::GET::measured(Handle document, STRING::Hot id) -> Extent {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return BACKEND::measured(document, node);
}

auto GUI::GET::origin(Handle document, STRING::Hot id) -> Position {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return BACKEND::origin(document, node);
}

auto GUI::GET::border(Handle document, STRING::Hot id) -> Float {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0.0f;
  return STATE::documents[document].nodes[node].properties.border;
}

auto GUI::GET::edge(Handle document, STRING::Hot id) -> GFX::Color {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.edge;
}

auto GUI::GET::hover(Handle document, STRING::Hot id) -> GFX::Color {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.hover;
}

auto GUI::GET::press(Handle document, STRING::Hot id) -> GFX::Color {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.press;
}

auto GUI::GET::glow(Handle document, STRING::Hot id) -> GFX::Color {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.glow;
}

auto GUI::GET::wash(Handle document, STRING::Hot id) -> GFX::Color {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return STATE::documents[document].nodes[node].properties.wash;
}

auto GUI::GET::pad(Handle document, STRING::Hot id) -> Float {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0.0f;
  return STATE::documents[document].nodes[node].properties.pad;
}

auto GUI::GET::size(Handle document, STRING::Hot id) -> Float {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0.0f;
  return STATE::documents[document].nodes[node].properties.size;
}

auto GUI::GET::wrap(Handle document, STRING::Hot id) -> Whole {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0;
  return STATE::documents[document].nodes[node].properties.wrap;
}

auto GUI::GET::opening(Handle document, STRING::Hot id) -> Flag {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return false;
  return STATE::documents[document].nodes[node].properties.open;
}

auto GUI::GET::clipping(Handle document, STRING::Hot id) -> Flag {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return false;
  return STATE::documents[document].nodes[node].properties.clip;
}
