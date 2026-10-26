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
#include <island/gui/events.internal.hpp>

namespace {

constexpr Float NUDGES = 100.0f;

}  // namespace

auto GUI::step(const Properties &properties) -> Float {
  if (properties.most <= properties.least) return 1.0f;
  const Float span = properties.most - properties.least;
  if (properties.steps == 0) return span / ::NUDGES;
  return span / static_cast<Float>(properties.steps);
}

auto GUI::nudge(Handle document, Float wheel, Flag shift) -> Flag {
  if (wheel == 0.0f) return false;
  auto &entry = STATE::documents[document];
  const auto hovered = entry.pointer.hovered;
  if (hovered == PARENTLESS || !entry.nodes[hovered].live) return false;
  const auto &node = entry.nodes[hovered];
  if (node.kind != "dial" || node.properties.graphic == Dial::NONE)
    return false;
  if (node.properties.seated) return false;
  Float notch = step(node.properties);
  if (shift) notch /= FINE;
  const Float value = node.properties.value + wheel * notch;
  turn(document, entry, hovered, value);
  return true;
}

void GUI::roll(Handle document, Float wheel, Flag control, Flag shift) {
  if (nudge(document, wheel, shift)) return;
  if (spin(document, wheel)) return;
  crank(document, wheel, control, shift);
}
