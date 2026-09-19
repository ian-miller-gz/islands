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
#include <island/gui/carry.internal.hpp>
#include <island/gui/events.internal.hpp>
#include <island/gui/nga.hpp>

namespace CARRY = GUI::SAC::CARRY;

namespace {

constexpr STRING::Hot BOX = "node";

auto pooled(Float at, Float low, Float high) -> Float {
  if (low != GUI::Walls::NONE && at < low) return low;
  if (high != GUI::Walls::NONE && at > high) return high;
  return at;
}

auto leading(const GUI::Document &entry, Whole deck) -> Whole {
  const auto &pointer = entry.pointer;
  const Whole index = pointer.pressed;
  if (!pointer.down || !pointer.dragged) return GUI::PARENTLESS;
  if (index == GUI::PARENTLESS || !entry.nodes[index].live)
    return GUI::PARENTLESS;
  if (entry.nodes[index].kind != ::BOX) return GUI::PARENTLESS;
  return entry.nodes[index].parent == deck ? index : GUI::PARENTLESS;
}

void gathered(GUI::Handle document, CARRY::Held &entry, Whole deck) {
  const GUI::Document &face = GUI::STATE::documents[document];
  if (!GUI::NGA::GET::selected(document, entry.leader.c_str())) return;
  const GUI::Position lead = face.pointer.standing;
  const STRING::Cold board = face.nodes[deck].id;
  for (const auto &id : GUI::NGA::GET::selections(document, board.c_str())) {
    Whole index = GUI::PARENTLESS;
    if (id == entry.leader || GUI::find(document, id.c_str(), index) != 0)
      continue;
    if (face.nodes[index].parent != deck) continue;
    const GUI::Position at = GUI::GET::position(document, id.c_str());
    entry.mates.push_back({id, {at.x - lead.x, at.y - lead.y}});
  }
}

void depth(GUI::Handle document, const CARRY::Held &entry, Float value) {
  if (entry.watched.held == entry.watched.seated) return;
  GUI::set(document, entry.leader.c_str(), GUI::Depth{value});
  for (const CARRY::Mate &mate : entry.mates)
    GUI::set(document, mate.id.c_str(), GUI::Depth{value});
}

}  // namespace

void CARRY::grab(GUI::Handle document, Held &entry, Whole deck) {
  const Whole index = ::leading(GUI::STATE::documents[document], deck);
  if (index == GUI::PARENTLESS) return;
  entry.leader = GUI::STATE::documents[document].nodes[index].id;
  entry.mates.clear();
  ::gathered(document, entry, deck);
  ::depth(document, entry, entry.watched.held);
}

void CARRY::tow(GUI::Handle document, const Held &entry) {
  const GUI::Position lead = GUI::GET::position(document, entry.leader.c_str());
  const GUI::Walls &walls = entry.watched.walls;
  for (const Mate &mate : entry.mates)
    GUI::set(
      document, mate.id.c_str(),
      GUI::Position{
        ::pooled(lead.x + mate.off.x, walls.west, walls.east),
        ::pooled(lead.y + mate.off.y, walls.north, walls.south)});
}

void CARRY::rest(GUI::Handle document, const Held &entry) {
  ::depth(document, entry, entry.watched.seated);
}
