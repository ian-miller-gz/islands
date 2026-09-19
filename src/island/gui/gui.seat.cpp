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
#include <island/gui/backend.hpp>

namespace {

auto rings(const GUI::Document &entry, Whole index, Whole referent) -> Flag {
  Whole at = referent;
  for (Whole step = 0; step <= entry.nodes.size(); step += 1) {
    if (at == index) return true;
    const auto &beside = entry.nodes[at].properties.beside;
    if (beside.empty()) return false;
    const auto found = entry.ids.find(String(beside));
    if (found == entry.ids.end()) return false;
    at = found->second;
  }
  return true;
}

}  // namespace

auto GUI::set(Handle document, STRING::Hot id, Seat seat) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &entry = STATE::documents[document];
  if (!seat.beside.empty()) {
    Whole referent = PARENTLESS;
    if (find(document, seat.beside.c_str(), referent) != 0) return 1;
    if (referent == node || ::rings(entry, node, referent)) return 1;
  }
  auto &properties = entry.nodes[node].properties;
  if (
    properties.beside == seat.beside &&
    properties.side == static_cast<Whole>(seat.side) &&
    properties.air == seat.air)
    return 0;
  properties.beside = seat.beside;
  properties.side = static_cast<Whole>(seat.side);
  properties.air = seat.air;
  return mark(document, node);
}

auto GUI::GET::seat(Handle document, STRING::Hot id) -> Seat {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const auto &properties = STATE::documents[document].nodes[node].properties;
  return {
    properties.beside, static_cast<Side>(properties.side), properties.air};
}

auto GUI::GET::room(Handle document, STRING::Hot id) -> Room {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const Whole parent = STATE::documents[document].nodes[node].parent;
  const Position at = BACKEND::origin(document, node);
  const Extent box = BACKEND::measured(document, node);
  const Position corner =
    parent == PARENTLESS ? Position{} : BACKEND::origin(document, parent);
  const Extent around = parent == PARENTLESS
                          ? BACKEND::extent(document)
                          : BACKEND::measured(document, parent);
  return {
    at.x - corner.x, corner.x + around.w - (at.x + box.w), at.y - corner.y,
    corner.y + around.h - (at.y + box.h)};
}
