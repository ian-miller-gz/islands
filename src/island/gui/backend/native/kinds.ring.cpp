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
#include <island/gui/backend/native/kinds.ring.hpp>
#include <algorithm>
#include <cmath>
#include <functional>

namespace {

constexpr Float GAUGE = 0.1f;
constexpr Float MARK = 2.5f;
constexpr Float HALF = 0.5f;

struct Disc {
  GUI::Position centre;
  Float radius = 0.0f;
  Float thickness = 0.0f;
};

auto point(const Disc &disc, Float along) -> GUI::Position {
  const Float angle = (along - HALF) * GUI::KINDS::SWEEP;
  return {
    disc.centre.x + disc.radius * std::sin(angle),
    disc.centre.y - disc.radius * std::cos(angle)};
}

auto lay(
  const Disc &disc, const GUI::KINDS::Arc &arc, const GFX::Color &ink,
  GUI::Element *elements) -> Whole {
  const Float count = static_cast<Float>(arc.pieces);
  for (Whole at = 0; at < arc.pieces; at += 1) {
    const Float from = std::lerp(arc.from, arc.to, at / count);
    const Float to = std::lerp(arc.from, arc.to, (at + 1) / count);
    elements[at] = GUI::KINDS::line(
      ::point(disc, from), ::point(disc, to), disc.thickness, ink);
    elements[at].caps = (from == 0.0f ? GUI::CAPS::NEAR : 0.0f) +
                        (to == 1.0f ? GUI::CAPS::FAR : 0.0f);
  }
  return arc.pieces;
}

}  // namespace

void GUI::KINDS::ring(const Node &node, const Slot &slot, Element *elements) {
  const auto &properties = node.properties;
  const Float side = slot.h - 2 * properties.border;
  const Float thickness = side * ::GAUGE;
  const Float mark = thickness * ::MARK;
  const ::Disc disc = {
    {slot.x + properties.border + side * ::HALF, slot.y + slot.h * ::HALF},
    (side - mark) * ::HALF,
    thickness};
  const auto track = faint(properties);
  auto runs = arcs(properties);
  std::ranges::stable_partition(runs, std::logical_not{}, &Arc::filled);
  Whole seat = 0;
  for (const auto &arc : runs)
    seat +=
      ::lay(disc, arc, arc.filled ? properties.press : track, elements + seat);
  const Flag bound = properties.most > properties.least;
  const auto spot =
    ::point(disc, bound ? share(properties, properties.value) : ::HALF);
  elements[PIECES] = {
    spot.x - mark * ::HALF, spot.y - mark * ::HALF, mark, mark, GUI::FULL,
    properties.press};
}

auto GUI::KINDS::inset(const Node &node, const Slot &slot) -> Float {
  const Float pad = node.properties.pad * slot.scale.w;
  if (node.row != DIAL || node.properties.graphic != Dial::DIAL) return pad;
  return pad + slot.h - node.properties.border;
}
