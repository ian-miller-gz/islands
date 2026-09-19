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
#include <island/gui/events.internal.hpp>
#include <island/gui/backend.hpp>
#include <algorithm>

namespace {

const Whole NODE = GUI::BACKEND::row("node");

}  // namespace

void GUI::NGA::select(Handle document, Document &entry, Whole hit) {
  if (hit == PARENTLESS || !selectable(entry, hit)) return;
  select(document, entry, board(entry, hit), {hit});
}

auto GUI::NGA::selectable(const Document &entry, Whole index) -> Flag {
  const auto &node = entry.nodes[index];
  if (!node.live) return false;
  if (node.row != ::NODE && node.properties.port == 0) return false;
  return board(entry, index) != PARENTLESS;
}

void GUI::NGA::select(
  Handle document, Document &entry, Whole deck, const Vector<Whole> &picks) {
  if (deck == PARENTLESS) return;
  Flag moved = false;
  for (Whole index = 0; index < entry.nodes.size(); index += 1) {
    if (!selectable(entry, index) || board(entry, index) != deck) continue;
    const Flag on = std::find(picks.begin(), picks.end(), index) != picks.end();
    if (entry.nodes[index].properties.selected == on) continue;
    const auto id = entry.nodes[index].id;
    if (set(document, id.c_str(), Selected{on}) == 0) moved = true;
  }
  if (!moved) return;
  record(entry, deck, Event::SELECTED);
  entry.pointer.selections.push_back(entry.nodes[deck].id);
}
