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
#include <algorithm>
#include <island/gui/nga.hpp>
#include <island/gui/gui.internal.hpp>

namespace {

auto onward(GUI::Handle document, const GUI::NGA::Link &link, Whole from)
  -> Whole {
  const auto &entry = GUI::STATE::documents[document];
  Whole out = GUI::PARENTLESS;
  Whole in = GUI::PARENTLESS;
  if (GUI::find(document, link.from.c_str(), out) != 0) return GUI::PARENTLESS;
  if (GUI::find(document, link.to.c_str(), in) != 0) return GUI::PARENTLESS;
  if (GUI::box(entry, out) != from) return GUI::PARENTLESS;
  return GUI::box(entry, in);
}

}  // namespace

auto GUI::NGA::GET::reaches(Handle document, STRING::Hot from, STRING::Hot to)
  -> Flag {
  Whole start = PARENTLESS;
  Whole goal = PARENTLESS;
  if (find(document, from, start) != 0) return false;
  if (find(document, to, goal) != 0) return false;
  const auto &entry = STATE::documents[document];
  const auto target = box(entry, goal);
  Vector<Whole> front = {box(entry, start)};
  Vector<Whole> met;
  while (!front.empty()) {
    const auto at = front.back();
    front.pop_back();
    for (const auto &link : entry.links) {
      const auto next = ::onward(document, link, at);
      if (next == PARENTLESS) continue;
      if (next == target) return true;
      if (std::find(met.begin(), met.end(), next) != met.end()) continue;
      met.push_back(next);
      front.push_back(next);
    }
  }
  return false;
}
