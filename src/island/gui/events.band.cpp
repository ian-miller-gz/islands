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
#include <island/gui/backend.hpp>

namespace {

auto shows(const GUI::Document &entry, Whole index) -> Flag {
  for (auto at = index; at != GUI::PARENTLESS; at = entry.nodes[at].parent)
    if (!entry.nodes[at].live || !entry.nodes[at].properties.visible)
      return false;
  return true;
}

auto inside(GUI::Handle document, Whole index, Float x, Float y) -> Flag {
  const auto corner = GUI::BACKEND::origin(document, index);
  const auto size = GUI::BACKEND::measured(document, index);
  return x >= corner.x && x < corner.x + size.w && y >= corner.y &&
         y < corner.y + size.h;
}

auto innermost(
  GUI::Handle document, const GUI::Document &entry, STRING::Hot kind,
  Whole deck, Float x, Float y) -> Whole {
  Whole found = GUI::PARENTLESS;
  for (Whole index = 0; index < entry.nodes.size(); index += 1) {
    if (kind != nullptr && entry.nodes[index].kind != kind) continue;
    if (!::shows(entry, index)) continue;
    if (deck != GUI::PARENTLESS && GUI::board(entry, index) != deck) continue;
    if (::inside(document, index, x, y)) found = index;
  }
  return found;
}

}  // namespace

auto GUI::decked(Handle document, const Document &entry, Float x, Float y)
  -> Whole {
  return ::innermost(document, entry, "graph", PARENTLESS, x, y);
}

auto GUI::boxed(
  Handle document, const Document &entry, Whole deck, Float x,
  Float y) -> Whole {
  return ::innermost(document, entry, "node", deck, x, y);
}

auto GUI::under(Handle document, const Document &entry, Float x, Float y)
  -> Whole {
  return ::innermost(document, entry, nullptr, PARENTLESS, x, y);
}

auto GUI::taken(const Document &entry, Whole hit, Whole deck) -> Flag {
  return hit != PARENTLESS && board(entry, hit) != deck;
}

void GUI::spread(Document &entry, Float x, Float y) {
  auto &pointer = entry.pointer;
  if (pointer.reach.x == x && pointer.reach.y == y) return;
  pointer.reach = {x, y};
  entry.dirty = true;
}

void GUI::gather(Handle document, Document &entry) {
  auto &pointer = entry.pointer;
  const auto deck = pointer.banding;
  if (deck == PARENTLESS) return;
  const auto caught = NGA::band(entry);
  pointer.banding = PARENTLESS;
  entry.restructured = true;
  entry.dirty = true;
  Vector<Whole> picks;
  for (Whole index = 0; index < entry.nodes.size(); index += 1)
    if (
      NGA::selectable(entry, index) && board(entry, index) == deck &&
      NGA::caught(document, index, caught))
      picks.push_back(index);
  NGA::select(document, entry, deck, picks);
}
