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

namespace {

using Act = void (*)(GUI::Handle, GUI::Document &, Whole);

struct Row {
  STRING::Hot kind;
  Act act;
};

void activate(GUI::Handle, GUI::Document &entry, Whole hit) {
  if (!GUI::doubles(entry.pointer, hit, 0)) return;
  entry.pointer.armed = GUI::PARENTLESS;
  GUI::record(entry, hit, GUI::Event::ACTIVATED);
  entry.pointer.activations.push_back(entry.nodes[hit].id);
}

void take(GUI::Handle, GUI::Document &entry, Whole hit) {
  if (!GUI::NGA::selectable(entry, hit)) return;
  if (!GUI::doubles(entry.pointer, hit, 0)) return;
  entry.pointer.armed = GUI::PARENTLESS;
  const Whole deck = GUI::board(entry, hit);
  GUI::record(entry, deck, GUI::Event::ACTIVATED);
  entry.pointer.activations.push_back(entry.nodes[deck].id);
}

constexpr Row ROWS[] = {
  {"dial", GUI::restore}, {"grip", ::activate}, {"node", ::take}};

}  // namespace

void GUI::doubled(Handle document, Document &entry, Whole hit) {
  for (const ::Row &row : ::ROWS)
    if (entry.nodes[hit].kind == row.kind) return row.act(document, entry, hit);
}
