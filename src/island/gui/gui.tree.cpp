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

auto GUI::live(Handle document) -> Flag {
  return document < STATE::documents.size() && STATE::documents[document].live;
}

auto GUI::find(Handle document, STRING::Hot id, Whole &node) -> Status {
  if (!live(document)) return 1;
  const auto &ids = STATE::documents[document].ids;
  const auto found = ids.find(id);
  if (found == ids.end()) return 1;
  node = found->second;
  return 0;
}

auto GUI::mark(Handle document, Whole node) -> Status {
  auto &entry = STATE::documents[document];
  if (!entry.nodes[node].dirty) entry.dirties.push_back(node);
  entry.nodes[node].dirty = true;
  entry.dirty = true;
  return 0;
}

void GUI::clean(Handle document) {
  if (!live(document)) return;
  auto &entry = STATE::documents[document];
  if (entry.restructured)
    for (auto &node : entry.nodes) node.dirty = false;
  else
    for (const Whole node : entry.dirties) entry.nodes[node].dirty = false;
  entry.dirties.clear();
  entry.dirty = false;
  entry.restructured = false;
}
