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
#include <island/gui/gui.internal.hpp>

auto GUI::NGA::clear(Handle document, STRING::Hot id) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  retire(STATE::documents[document], id);
  return 0;
}

void GUI::retire(Document &entry, STRING::Hot id) {
  const auto before = entry.links.size();
  for (Whole at = entry.links.size(); at > 0; at -= 1) {
    const auto &link = entry.links[at - 1];
    if (link.from != id && link.to != id) continue;
    entry.links.erase(entry.links.begin() + (at - 1));
  }
  if (entry.links.size() == before) return;
  entry.dirty = true;
  entry.restructured = true;
}
