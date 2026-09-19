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

namespace {

auto candidate(const GUI::Document &entry, Whole held, Whole deck, Whole index)
  -> Flag {
  if (index == held || !entry.nodes[index].live) return false;
  if (entry.nodes[index].properties.port == 0) return false;
  if (GUI::board(entry, index) != deck) return false;
  return GUI::NGA::agree(entry, held, index);
}

auto wireable(const GUI::Document &entry, Whole deck, Whole index) -> Flag {
  return GUI::wires(entry, index) && GUI::board(entry, index) == deck;
}

}  // namespace

auto GUI::snap(Handle document, const Document &entry) -> Whole {
  const auto &pointer = entry.pointer;
  const auto held = pointer.wiring;
  if (held == PARENTLESS) return PARENTLESS;
  const auto deck = board(entry, held);
  if (deck == PARENTLESS) return PARENTLESS;
  Whole found = PARENTLESS;
  Float best = REACH * REACH;
  for (Whole index = 0; index < entry.nodes.size(); index += 1) {
    if (!::candidate(entry, held, deck, index)) continue;
    const auto there = NGA::centre(document, index);
    const Float x = there.x - pointer.reach.x;
    const Float y = there.y - pointer.reach.y;
    const Float span = x * x + y * y;
    if (span >= best) continue;
    found = index;
    best = span;
  }
  return found;
}

auto GUI::aim(
  Handle document, const Document &entry, Whole hit, Float x,
  Float y) -> Whole {
  if (wires(entry, hit)) return hit;
  if (hit != PARENTLESS) return PARENTLESS;
  const auto deck = decked(document, entry, x, y);
  if (deck == PARENTLESS) return PARENTLESS;
  Whole found = PARENTLESS;
  Float best = GRASP * GRASP;
  for (Whole index = 0; index < entry.nodes.size(); index += 1) {
    if (!::wireable(entry, deck, index)) continue;
    const auto there = NGA::centre(document, index);
    const Float dx = there.x - x;
    const Float dy = there.y - y;
    const Float span = dx * dx + dy * dy;
    if (span >= best) continue;
    found = index;
    best = span;
  }
  return found;
}
