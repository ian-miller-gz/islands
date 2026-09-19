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

auto GUI::set(Handle document, STRING::Hot id, Axis axis) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  properties.free = axis.free;
  properties.across = axis.across || axis.free;
  return 0;
}

auto GUI::GET::axis(Handle document, STRING::Hot id) -> Axis {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const auto &properties = STATE::documents[document].nodes[node].properties;
  return {properties.across, properties.free};
}

auto GUI::set(Handle document, STRING::Hot id, Carry carry) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  STATE::documents[document].nodes[node].properties.carried = carry.pointer;
  return 0;
}

auto GUI::GET::carry(Handle document, STRING::Hot id) -> Carry {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return {STATE::documents[document].nodes[node].properties.carried};
}
