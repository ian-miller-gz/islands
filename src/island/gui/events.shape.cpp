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
#include <island/window/backend.hpp>

void GUI::shape() {
  if (carried()) return;
  Flag typing = false;
  for (const auto &entry : STATE::documents) {
    if (!entry.live || !entry.pointer.placed) continue;
    const auto hovered = entry.pointer.hovered;
    if (hovered == PARENTLESS || !entry.nodes[hovered].live) continue;
    if (typeable(entry.nodes[hovered])) typing = true;
  }
  WINDOW::shape(typing ? WINDOW::IBEAM : WINDOW::ARROW);
}
