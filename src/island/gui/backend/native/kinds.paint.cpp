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
#include <algorithm>

namespace {

constexpr Whole RING = 8;
constexpr Float DONGLE = 3.0f;

void skip(const GUI::Node &, const GUI::Slot &, GUI::Element *) {}

void fill(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  const auto &properties = node.properties;
  const Float rim = properties.border;
  const Float width = slot.w - 2 * rim;
  Float from = slot.x + rim + (width - DONGLE) / 2;
  Float span = DONGLE;
  if (properties.most > properties.least) {
    const Float across = (properties.value - properties.least) /
                         (properties.most - properties.least);
    from = slot.x + rim;
    span = width * std::clamp(across, 0.0f, 1.0f);
  }
  elements[5] = {from,      slot.y + rim,    span, slot.h - 2 * rim,
                 GUI::FULL, properties.press};
}

void turn(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  GUI::KINDS::ring(node, slot, elements + RING);
}

}  // namespace

const GUI::KINDS::Paint GUI::KINDS::PAINTS[] = {::skip, ::fill, ::turn};
