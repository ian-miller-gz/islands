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

namespace {

constexpr Float COARSE = 24.0f;

}  // namespace

auto GUI::quantize(
  const Properties &properties, const INPUT::Chord &chord,
  Float value) -> Float {
  if (!chord.control || properties.steps > 0) return value;
  if (properties.most <= properties.least) return value;
  const Float step = (properties.most - properties.least) / ::COARSE;
  const Float steps = std::round((value - properties.least) / step);
  return properties.least + steps * step;
}

void GUI::fill(Handle document, Document &entry, Whole index, Float x) {
  const auto &properties = entry.nodes[index].properties;
  const auto corner = BACKEND::origin(document, index);
  const auto size = BACKEND::measured(document, index);
  if (size.w <= 0.0f) return;
  const Float across = (x - corner.x) / size.w;
  const Float value =
    properties.least + across * (properties.most - properties.least);
  turn(
    document, entry, index, quantize(properties, entry.pointer.chord, value));
}

auto GUI::cancel(
  Handle document, Document &entry, const INPUT::KEYS::Event &event) -> Flag {
  if (event.action != INPUT::KEYS::TEXT) return false;
  if (event.codepoint != INPUT::KEYS::ESCAPE) return false;
  auto &pointer = entry.pointer;
  const auto index = pointer.pressed;
  if (index == PARENTLESS || !pointer.dragged) return false;
  if (!entry.nodes[index].live || entry.nodes[index].kind != "dial")
    return false;
  turn(document, entry, index, pointer.grabbed);
  pointer.pressed = PARENTLESS;
  pointer.dragged = false;
  return true;
}
