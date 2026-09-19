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

void GUI::crank(Handle document, Float wheel, Flag control) {
  if (wheel == 0.0f) return;
  auto &entry = STATE::documents[document];
  auto &pointer = entry.pointer;
  const auto spot = pointer.spot;
  const auto deck = decked(document, entry, spot.x, spot.y);
  if (deck == PARENTLESS) return;
  const auto &worn = entry.nodes[deck].properties;
  const auto board = NGA::scaled(document, deck);
  const Float across = board.w * (worn.zoom.w > 0.0f ? worn.zoom.w : 1.0f);
  const Float down = board.h * (worn.zoom.h > 0.0f ? worn.zoom.h : 1.0f);
  const auto corner = BACKEND::origin(document, deck);
  pointer.spun = deck;
  pointer.turned = {
    wheel, control, worn.pan.x + (spot.x - corner.x) / across,
    worn.pan.y + (spot.y - corner.y) / down};
}

auto GUI::NGA::GET::wheel(Handle document, STRING::Hot id) -> Wheel {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  auto &pointer = STATE::documents[document].pointer;
  if (pointer.spun != node) return {};
  const auto turned = pointer.turned;
  pointer.turned = {};
  pointer.spun = PARENTLESS;
  return turned;
}
