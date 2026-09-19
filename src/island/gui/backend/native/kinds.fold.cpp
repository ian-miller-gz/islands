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
#include <island/gui/backend/native/kinds.internal.hpp>
#include <island/graphics/text/text.hpp>
#include <algorithm>
#include <cmath>

void GUI::KINDS::fold(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  constexpr Float STROKE = 1.0f;
  constexpr Float THICK = 2.0f;
  constexpr Float CLEAR = 4.0f;
  constexpr Float SHARE = 0.5f;
  const auto &properties = node.properties;
  const auto &ink = properties.edge;
  const Flag worn = properties.fold != Fold::NONE;
  const Flag closed = properties.fold == Fold::CLOSED;
  const Float side = worn ? std::round(GFX::TEXT::GET::advance(
                              BACKEND::NATIVE::lettered(node, slot)))
                          : 0.0f;
  const Float rim = worn ? STROKE : 0.0f;
  const Float thick = worn ? THICK : 0.0f;
  const Float bar = std::round(side * SHARE);
  const Float x = std::round(
    std::max(slot.x, slot.x + (properties.pad - CLEAR) * slot.scale.w - side));
  const Float y = std::round(slot.y + (slot.h - side) / 2.0f);
  const Float stop = std::round((side - bar) / 2.0f);
  const Float half = std::round((side - thick) / 2.0f);
  elements[0] = {x, y, side, rim, GUI::FULL, ink};
  elements[1] = {x, y + side - rim, side, rim, GUI::FULL, ink};
  elements[2] = {x, y + rim, rim, side - 2 * rim, GUI::FULL, ink};
  elements[3] = {x + side - rim, y + rim, rim, side - 2 * rim, GUI::FULL, ink};
  elements[4] = {x + stop, y + half, bar, thick, GUI::FULL, ink};
  elements[5] = {x + half,  y + stop, thick, closed ? bar : 0.0f,
                 GUI::FULL, ink};
}

void GUI::KINDS::item(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  button(node, slot, elements);
  fold(node, slot, elements + 5);
}
