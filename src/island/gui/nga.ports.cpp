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

auto GUI::NGA::GET::ports(Handle document, STRING::Hot id)
  -> Vector<STRING::Cold> {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const auto &entry = STATE::documents[document];
  Vector<STRING::Cold> facing;
  for (Whole index = node; index < entry.nodes.size(); index += 1) {
    const auto &widget = entry.nodes[index];
    if (!widget.live || !GUI::under(entry, index, node)) continue;
    if (widget.properties.port != Port::NONE) facing.push_back(widget.id);
  }
  return facing;
}

auto GUI::NGA::rank(const Document &entry, Whole index) -> Whole {
  const auto home = GUI::box(entry, index);
  const auto side = entry.nodes[index].properties.port;
  Whole rank = 0;
  for (Whole at = home; at < index; at += 1) {
    const auto &widget = entry.nodes[at];
    if (!widget.live || widget.properties.port != side) continue;
    if (GUI::under(entry, at, home)) rank += 1;
  }
  return rank;
}
