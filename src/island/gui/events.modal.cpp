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

auto shown(const GUI::Document &entry, Whole index) -> Flag {
  for (auto at = index; at != GUI::PARENTLESS; at = entry.nodes[at].parent)
    if (!entry.nodes[at].live || !entry.nodes[at].properties.visible)
      return false;
  return true;
}

auto standing(const GUI::Document &entry, Whole index) -> Float {
  Float z = 0.0f;
  for (auto at = index; at != GUI::PARENTLESS; at = entry.nodes[at].parent)
    z += entry.nodes[at].properties.z;
  return z;
}

}  // namespace

void GUI::veil(Document &entry) {
  entry.modal = PARENTLESS;
  Float top = 0.0f;
  for (Whole index = 0; index < entry.nodes.size(); index += 1) {
    if (!entry.nodes[index].properties.modal || !::shown(entry, index))
      continue;
    const auto z = ::standing(entry, index);
    if (entry.modal != PARENTLESS && z < top) continue;
    entry.modal = index;
    top = z;
  }
}

auto GUI::veiled(const Document &entry, Whole hit) -> Flag {
  return entry.modal != PARENTLESS && hit != PARENTLESS &&
         !under(entry, hit, entry.modal);
}
