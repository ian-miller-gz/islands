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
#include <island/gui/nga.internal.hpp>
#include <algorithm>

void GUI::shift(Handle document, Document &entry, Float x, Float y) {
  auto &pointer = entry.pointer;
  const auto index = pointer.pressed;
  if (index == PARENTLESS || !entry.nodes[index].live) return;
  if (entry.nodes[index].kind != "node") return;
  const Float across = x - pointer.grab.x;
  const Float down = y - pointer.grab.y;
  if (
    !pointer.dragged && std::abs(across) < THRESHOLD &&
    std::abs(down) < THRESHOLD)
    return;
  if (!pointer.dragged) pointer.armed = PARENTLESS;
  pointer.dragged = true;
  const auto scale = NGA::scaled(document, index);
  const auto id = entry.nodes[index].id;
  const auto before = entry.nodes[index].properties.position;
  const Position landed = {
    pointer.standing.x + across / scale.w, pointer.standing.y + down / scale.h};
  if (GUI::set(document, id.c_str(), landed) != 0) return;
  if (entry.nodes[index].properties.position == before) return;
  auto &moves = pointer.moves;
  if (std::find(moves.begin(), moves.end(), id) != moves.end()) return;
  record(entry, index, Event::MOVED);
  moves.push_back(id);
}
