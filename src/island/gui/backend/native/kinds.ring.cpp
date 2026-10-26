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
#include <cmath>

namespace {

constexpr Float GAUGE = 0.1f;
constexpr Float MARK = 2.5f;
constexpr Float HALF = 0.5f;

struct Fill {
  Float from = 1.0f;
  Float to = 0.0f;
};

auto share(const GUI::Properties &properties, Float value) -> Float {
  const Float span = properties.most - properties.least;
  return std::clamp((value - properties.least) / span, 0.0f, 1.0f);
}

auto fill(const GUI::Properties &properties) -> Fill {
  if (properties.most <= properties.least) return {};
  const Flag centred = properties.least < 0.0f && properties.most > 0.0f;
  const Float origin = centred ? share(properties, 0.0f) : 0.0f;
  const Float value = share(properties, properties.value);
  return {std::min(origin, value), std::max(origin, value)};
}

auto point(GUI::Position centre, Float radius, Float along) -> GUI::Position {
  const Float angle = (along - HALF) * GUI::KINDS::SWEEP;
  return {
    centre.x + radius * std::sin(angle), centre.y - radius * std::cos(angle)};
}

}  // namespace

void GUI::KINDS::ring(const Node &node, const Slot &slot, Element *elements) {
  const auto &properties = node.properties;
  const Float side = slot.h - 2 * properties.border;
  const Float thickness = side * ::GAUGE;
  const Float mark = thickness * ::MARK;
  const Float radius = (side - mark) * ::HALF;
  const Position centre = {
    slot.x + properties.border + side * ::HALF, slot.y + slot.h * ::HALF};
  const auto filled = ::fill(properties);
  const Float count = static_cast<Float>(PIECES);
  for (Whole at = 0; at < PIECES; at += 1) {
    const Float from = static_cast<Float>(at) / count;
    const Float to = static_cast<Float>(at + 1) / count;
    const Float middle = (from + to) * ::HALF;
    const Flag inside = middle >= filled.from && middle <= filled.to;
    elements[at] = piece(
      ::point(centre, radius, from), ::point(centre, radius, to), thickness,
      inside ? properties.press : properties.edge);
  }
  const Flag bound = properties.most > properties.least;
  const auto spot = ::point(
    centre, radius, bound ? ::share(properties, properties.value) : ::HALF);
  elements[PIECES] = {
    spot.x - mark * ::HALF, spot.y - mark * ::HALF, mark, mark, GUI::FULL,
    properties.press};
}

auto GUI::KINDS::inset(const Node &node, const Slot &slot) -> Float {
  const Float pad = node.properties.pad * slot.scale.w;
  if (node.row != DIAL || node.properties.graphic != Dial::DIAL) return pad;
  return pad + slot.h - node.properties.border;
}
