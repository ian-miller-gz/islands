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
#include <island/gui/nga.hpp>
#include <island/gui/gui.internal.hpp>

auto GUI::NGA::set(Handle document, STRING::Hot id, Selected selected)
  -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.selected == selected.on) return 0;
  properties.selected = selected.on;
  return mark(document, node);
}

auto GUI::NGA::set(Handle document, STRING::Hot id, Choosing choosing)
  -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.banded == choosing.band) return 0;
  properties.banded = choosing.band;
  return mark(document, node);
}

auto GUI::NGA::GET::choosing(Handle document, STRING::Hot id) -> Choosing {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  return {STATE::documents[document].nodes[node].properties.banded};
}

auto GUI::NGA::GET::selected(Handle document, STRING::Hot id) -> Flag {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return false;
  return STATE::documents[document].nodes[node].properties.selected;
}

auto GUI::NGA::GET::selections(Handle document, STRING::Hot id)
  -> Vector<STRING::Cold> {
  if (!live(document)) return {};
  const auto &entry = STATE::documents[document];
  Whole deck = PARENTLESS;
  if (id[0] != '\0' && find(document, id, deck) != 0) return {};
  Vector<STRING::Cold> standing;
  for (Whole index = 0; index < entry.nodes.size(); index += 1) {
    const auto &node = entry.nodes[index];
    if (!node.live || !node.properties.selected) continue;
    if (deck != PARENTLESS && board(entry, index) != deck) continue;
    standing.push_back(node.id);
  }
  return standing;
}
