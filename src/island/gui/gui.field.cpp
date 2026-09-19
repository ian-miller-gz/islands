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
#include <island/gui/gui.internal.hpp>

namespace {

void wear(GUI::Handle document, Whole index, Flag seated) {
  auto &entry = GUI::STATE::documents[document];
  if (index >= entry.nodes.size()) return;
  auto &properties = entry.nodes[index].properties;
  if (properties.seated == seated) return;
  properties.seated = seated;
  GUI::mark(document, index);
}

}  // namespace

void GUI::seat(Handle document, Whole index, Flag seated) {
  if (!live(document)) return;
  auto &entry = STATE::documents[document];
  const auto held = entry.editing;
  entry.editing = index;
  entry.seated = seated;
  if (held != index) ::wear(document, held, false);
  ::wear(document, index, seated);
}

auto GUI::set(Handle document, STRING::Hot id, Caret caret) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  const Whole end = properties.text.size();
  const Whole at = caret.at < end ? caret.at : end;
  const Whole from = caret.from < end ? caret.from : end;
  if (properties.caret == at && properties.selection == from) return 0;
  properties.caret = at;
  properties.selection = from;
  return mark(document, node);
}

auto GUI::GET::caret(Handle document, STRING::Hot id) -> Caret {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const auto &properties = STATE::documents[document].nodes[node].properties;
  return {properties.caret, properties.selection};
}

auto GUI::edit(Handle document, STRING::Hot id) -> Status {
  if (!live(document)) return 1;
  auto &entry = STATE::documents[document];
  if (*id == '\0') {
    seat(document, PARENTLESS, false);
    return 0;
  }
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  const auto &kind = entry.nodes[node].kind;
  if (kind != "field" && kind != "dial") return 1;
  seat(document, node, true);
  return 0;
}

auto GUI::GET::editing(Handle document) -> STRING::Cold {
  if (!live(document)) return {};
  const auto &entry = STATE::documents[document];
  const auto node = entry.editing;
  if (node == PARENTLESS || !entry.nodes[node].live) return {};
  return entry.nodes[node].id;
}
