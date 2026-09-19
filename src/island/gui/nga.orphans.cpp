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

auto among(const Vector<Whole> &boxes, Whole box) -> Flag {
  for (const Whole held : boxes)
    if (held == box) return true;
  return false;
}

void joins(GUI::Handle document, Vector<Join> &made) {
  const auto &entry = GUI::STATE::documents[document];
  for (const auto &link : entry.links) {
    Whole from = GUI::PARENTLESS;
    Whole to = GUI::PARENTLESS;
    if (GUI::find(document, link.from.c_str(), from) != 0) continue;
    if (GUI::find(document, link.to.c_str(), to) != 0) continue;
    made.push_back({GUI::box(entry, from), GUI::box(entry, to)});
  }
}

auto fed(const Vector<Join> &made, Whole box) -> Flag {
  for (const auto &join : made)
    if (join.to == box) return true;
  return false;
}

void stands(const Vector<Join> &made, Whole gone, Vector<Whole> &held) {
  for (const auto &join : made)
    for (const Whole box : {join.from, join.to})
      if (box != gone && !::fed(made, box) && !::among(held, box))
        held.push_back(box);
  for (Flag grew = true; grew;) {
    grew = false;
    for (const auto &join : made) {
      if (join.to == gone || !::among(held, join.from)) continue;
      if (::among(held, join.to)) continue;
      held.push_back(join.to);
      grew = true;
    }
  }
}

}  // namespace

auto GUI::NGA::GET::orphans(
  Handle document, STRING::Hot id, Vector<STRING::Cold> &boxes) -> Status {
  boxes.clear();
  if (!live(document)) return 1;
  Whole index = PARENTLESS;
  if (find(document, id, index) != 0) return 1;
  const auto &entry = STATE::documents[document];
  const Whole gone = box(entry, index);
  Vector<Join> made;
  ::joins(document, made);
  Vector<Whole> before;
  Vector<Whole> after;
  ::stands(made, PARENTLESS, before);
  ::stands(made, gone, after);
  for (Whole at = 0; at < entry.nodes.size(); at += 1) {
    if (at == gone || !entry.nodes[at].live) continue;
    if (entry.nodes[at].row != ::NODE) continue;
    if (::among(before, at) && !::among(after, at))
      boxes.push_back(entry.nodes[at].id);
  }
  return 0;
}
