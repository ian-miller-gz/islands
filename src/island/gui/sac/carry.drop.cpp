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

namespace CARRY = GUI::SAC::CARRY;

namespace {

auto watched(GUI::Handle document, const CARRY::Held &entry) -> Whole {
  Whole deck = GUI::PARENTLESS;
  if (entry.watched.board.empty()) return GUI::PARENTLESS;
  if (GUI::find(document, entry.watched.board.c_str(), deck) != 0)
    return GUI::PARENTLESS;
  return deck;
}

auto stranded(GUI::Handle document, CARRY::Held &entry) -> Flag {
  Whole index = GUI::PARENTLESS;
  if (entry.leader.empty()) return false;
  if (GUI::find(document, entry.leader.c_str(), index) == 0) return false;
  entry.leader.clear();
  entry.mates.clear();
  return true;
}

void fallen(
  GUI::Handle document, const STRING::Cold &id, Vector<CARRY::Drop> &set) {
  Whole index = GUI::PARENTLESS;
  if (GUI::find(document, id.c_str(), index) != 0) return;
  set.push_back({id, GUI::GET::position(document, id.c_str())});
}

void land(GUI::Handle document, CARRY::Held &entry) {
  CARRY::rest(document, entry);
  for (const CARRY::Mate &mate : entry.mates)
    ::fallen(document, mate.id, entry.dropped);
  ::fallen(document, entry.leader, entry.dropped);
  entry.leader.clear();
  entry.mates.clear();
}

}  // namespace

void CARRY::tend() {
  for (auto &face : holds()) {
    const Handle document = face.first;
    if (!GUI::live(document)) continue;
    Held &entry = holding(document);
    entry.dropped.clear();
    const Whole deck = ::watched(document, entry);
    if (deck == GUI::PARENTLESS || ::stranded(document, entry)) continue;
    if (entry.leader.empty()) {
      grab(document, entry, deck);
      if (entry.leader.empty()) continue;
    } else if (!GUI::STATE::documents[document].pointer.down) {
      ::land(document, entry);
      continue;
    }
    tow(document, entry);
  }
}
