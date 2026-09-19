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
#include <island/gui/backend.hpp>
#include <island/gui/gui.internal.hpp>

namespace {

const Whole NODE = GUI::BACKEND::row("node");

struct Join {
  Whole from = 0;
  Whole to = 0;
};

auto unit(const GUI::Document &entry, Whole index) -> Flag {
  if (!entry.nodes[index].live) return false;
  if (entry.nodes[index].row == ::NODE) return true;
  return entry.nodes[index].properties.port != GUI::NGA::Port::NONE &&
         GUI::box(entry, index) == index;
}

auto seat(Vector<Whole> &units, Whole index) -> Whole {
  for (Whole at = 0; at < units.size(); at += 1)
    if (units[at] == index) return at;
  units.push_back(index);
  return units.size() - 1;
}

void plan(
  GUI::Handle document, Whole root, Vector<Whole> &units, Vector<Join> &joins) {
  const auto &entry = GUI::STATE::documents[document];
  for (Whole index = 0; index < entry.nodes.size(); index += 1)
    if (::unit(entry, index) && GUI::under(entry, index, root))
      units.push_back(index);
  for (const auto &link : entry.links) {
    Whole from = GUI::PARENTLESS;
    Whole to = GUI::PARENTLESS;
    if (GUI::find(document, link.from.c_str(), from) != 0) continue;
    if (GUI::find(document, link.to.c_str(), to) != 0) continue;
    from = GUI::box(entry, from);
    to = GUI::box(entry, to);
    if (!GUI::under(entry, from, root) || !GUI::under(entry, to, root))
      continue;
    joins.push_back({::seat(units, from), ::seat(units, to)});
  }
}

auto freest(
  const Vector<Whole> &units, const Vector<Flag> &settled,
  const Vector<Join> &joins) -> Whole {
  for (Whole at = 0; at < units.size(); at += 1) {
    if (settled[at]) continue;
    Flag fed = false;
    for (const auto &join : joins)
      if (join.to == at && !settled[join.from]) fed = true;
    if (!fed) return at;
  }
  return GUI::PARENTLESS;
}

}  // namespace

auto GUI::NGA::GET::order(
  Handle document, STRING::Hot id, Vector<STRING::Cold> &boxes) -> Status {
  boxes.clear();
  if (!live(document)) return 1;
  Whole root = PARENTLESS;
  if (id[0] != '\0' && find(document, id, root) != 0) return 1;
  const auto &entry = STATE::documents[document];
  Vector<Whole> units;
  Vector<Join> joins;
  ::plan(document, root, units, joins);
  Vector<Flag> settled(units.size(), false);
  for (Whole placed = 0; placed < units.size(); placed += 1) {
    const auto next = ::freest(units, settled, joins);
    if (next == PARENTLESS) {
      boxes.clear();
      return 1;
    }
    settled[next] = true;
    if (entry.nodes[units[next]].row == ::NODE)
      boxes.push_back(entry.nodes[units[next]].id);
  }
  return 0;
}
