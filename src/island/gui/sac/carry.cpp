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
#include <island/gui/gui.internal.hpp>
#include <island/gui/nga.hpp>
#include <island/gui/sac.hpp>
#include <logger.hpp>

static const String category = "~/island/gui::";

namespace {
Map<GUI::Handle, GUI::SAC::CARRY::Held> standing;
Flag told = false;
}  // namespace

auto GUI::SAC::CARRY::holds() -> Map<Handle, Held> & { return ::standing; }

auto GUI::SAC::CARRY::holding(Handle document) -> Held & {
  Held &entry = ::standing[document];
  const Whole current = GUI::GET::generation(document);
  if (entry.generation != current) {
    entry = {};
    entry.generation = current;
  }
  return entry;
}

auto GUI::SAC::CARRY::refused() -> Flag {
  if (GUI::SAC::GET::adopted(GUI::SAC::CARRY::NAME)) return false;
  if (!::told)
    LOGGER::get(category + "carry")
      .error(
        "The carry is not designated: add `carry: true` under the "
        "manifest's `gui:` block (or adopt it through GUI::SAC::adopt).");
  ::told = true;
  return true;
}

auto GUI::SAC::CARRY::watch(Handle document, const Watch &watched) -> Status {
  if (!live(document)) return 1;
  if (refused()) return 1;
  Held &entry = holding(document);
  if (entry.watched.board != watched.board) {
    entry.leader.clear();
    entry.mates.clear();
  }
  entry.watched = watched;
  return 0;
}

auto GUI::SAC::CARRY::follow(Handle document, const Vector<String> &ids)
  -> Status {
  if (!live(document)) return static_cast<Status>(ids.size());
  if (refused()) return static_cast<Status>(ids.size());
  const STRING::Cold board = holding(document).watched.board;
  if (board.empty()) return static_cast<Status>(ids.size());
  for (const auto &standing : NGA::GET::selections(document, board.c_str()))
    NGA::set(document, standing.c_str(), NGA::Selected{false});
  Status missed = 0;
  for (const auto &id : ids)
    if (NGA::set(document, id.c_str(), NGA::Selected{true}) != 0) missed += 1;
  return missed;
}

auto GUI::SAC::CARRY::GET::carried(Handle document) -> STRING::Cold {
  if (!live(document)) return {};
  return holding(document).leader;
}

auto GUI::SAC::CARRY::GET::dragged(Handle document, STRING::Hot id) -> Flag {
  if (!live(document)) return false;
  const Held &entry = holding(document);
  if (entry.leader.empty()) return false;
  if (entry.leader == id) return true;
  for (const Mate &mate : entry.mates)
    if (mate.id == id) return true;
  return false;
}

auto GUI::SAC::CARRY::GET::dropped(Handle document) -> Vector<Drop> {
  if (!live(document)) return {};
  return holding(document).dropped;
}
