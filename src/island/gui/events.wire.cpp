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
#include <cmath>

namespace {

auto faces(const GUI::Document &entry, Whole index) -> Flag {
  return index != GUI::PARENTLESS && entry.nodes[index].live &&
         entry.nodes[index].properties.port != 0;
}

void reseat(GUI::Document &entry) {
  if (GUI::NGA::pending(entry) != GUI::PARENTLESS) entry.restructured = true;
}

auto catches(const GUI::Document &entry, Whole deck) -> Flag {
  for (Whole index = 0; index < entry.nodes.size(); index += 1)
    if (GUI::NGA::selectable(entry, index) && GUI::board(entry, index) == deck)
      return true;
  return false;
}

}  // namespace

auto GUI::wires(const Document &entry, Whole index) -> Flag {
  return ::faces(entry, index) && entry.nodes[index].kind != "node" &&
         !entry.nodes[index].properties.mute &&
         board(entry, index) != PARENTLESS;
}

void GUI::hitch(Handle document, Document &entry, Whole hit, Float x, Float y) {
  auto &pointer = entry.pointer;
  const auto held = aim(document, entry, hit, x, y);
  if (held != PARENTLESS) {
    if (held != hit) {
      pointer.pressed = held;
      grab(entry, held, x, y);
    }
    pointer.wiring = held;
    pointer.reach = {x, y};
    pointer.snapped = PARENTLESS;
    ::reseat(entry);
    entry.dirty = true;
    return;
  }
  if (hit != PARENTLESS) return;
  const auto deck = decked(document, entry, x, y);
  if (deck == PARENTLESS) return;
  pointer.grab = {x, y};
  if (spanned(document, entry, deck, x, y)) {
    pointer.cutting = deck;
    return;
  }
  if (!::catches(entry, deck)) return;
  pointer.banding = deck;
  pointer.reach = {x, y};
  entry.restructured = true;
  entry.dirty = true;
}

void GUI::trail(Handle document, Document &entry, Float x, Float y) {
  auto &pointer = entry.pointer;
  if (pointer.cutting != PARENTLESS) return sever(document, entry, x, y);
  if (pointer.banding != PARENTLESS) return spread(entry, x, y);
  if (pointer.wiring == PARENTLESS) return;
  if (pointer.reach.x == x && pointer.reach.y == y) return;
  pointer.reach = {x, y};
  if (
    std::abs(x - pointer.grab.x) >= THRESHOLD ||
    std::abs(y - pointer.grab.y) >= THRESHOLD)
    pointer.dragged = true;
  pointer.snapped = snap(document, entry);
  entry.dirty = true;
}

void GUI::fasten(Handle document, Document &entry, Whole hit) {
  auto &pointer = entry.pointer;
  pointer.cutting = PARENTLESS;
  gather(document, entry);
  const auto held = pointer.wiring;
  if (held == PARENTLESS) return;
  const auto aimed = pointer.snapped;
  ::reseat(entry);
  pointer.wiring = PARENTLESS;
  pointer.snapped = PARENTLESS;
  entry.dirty = true;
  const auto landed = ::faces(entry, hit) ? hit : aimed;
  if (landed == PARENTLESS) return;
  const auto from = entry.nodes[held].id;
  const auto to = entry.nodes[landed].id;
  if (NGA::connect(document, from.c_str(), to.c_str()) != 0) return;
  pointer.joins.push_back({from, to, ""});
  record(entry, board(entry, landed), Event::JOINED);
}
