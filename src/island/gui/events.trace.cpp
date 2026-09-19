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
#include <island/gui/backend.hpp>
#include <cmath>

void GUI::land(Handle document, Document &entry, Whole index, Float x) {
  const auto id = entry.nodes[index].id;
  const auto at = BACKEND::caret(document, index, x);
  GUI::set(document, id.c_str(), Caret{at, at});
}

void GUI::trace(Handle document, Document &entry, Float x) {
  auto &pointer = entry.pointer;
  const auto index = pointer.pressed;
  if (index == PARENTLESS || !entry.nodes[index].live) return;
  const auto &node = entry.nodes[index];
  if (!typeable(node)) return;
  if (node.kind == "dial" && !node.properties.seated) return;
  if (!pointer.dragged && std::abs(x - pointer.grab.x) < GUI::THRESHOLD) return;
  if (!pointer.dragged) pointer.armed = PARENTLESS;
  pointer.dragged = true;
  const auto id = node.id;
  const auto anchor = BACKEND::caret(document, index, pointer.grab.x);
  const auto landed = BACKEND::caret(document, index, x);
  seat(document, index, true);
  GUI::set(document, id.c_str(), Caret{landed, anchor});
}
