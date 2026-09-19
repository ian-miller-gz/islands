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

using Curve = Float (*)(Float);
constexpr Curve CURVES[] = {
  [](Float along) { return along; },
  [](Float along) { return along * along; },
  [](Float along) { return 1.0f - (1.0f - along) * (1.0f - along); },
  [](Float along) { return along * along * (3.0f - 2.0f * along); },
  [](Float along) { return along < 1.0f ? 0.0f : 1.0f; },
};
constexpr Whole SHAPES = sizeof(CURVES) / sizeof(*CURVES);

auto piece(
  GUI::Position from, GUI::Position to, Float thickness,
  const GFX::Color &ink) -> GUI::Element {
  if (from.x == to.x && from.y == to.y) return {};
  const Float half = thickness / 2.0f;
  return {
    std::min(from.x, to.x) - half,
    std::min(from.y, to.y) - half,
    std::abs(to.x - from.x) + thickness,
    std::abs(to.y - from.y) + thickness,
    GUI::FULL,
    ink};
}

}  // namespace

void GUI::KINDS::stroke(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  const auto &properties = node.properties;
  const Float thickness = properties.border * slot.scale.w;
  const auto ink = washed(properties, properties.color);
  const Float run = properties.to.x * slot.scale.w;
  const Float drop = properties.to.y * slot.scale.h;
  const auto curve =
    ::CURVES[properties.shape < ::SHAPES ? properties.shape : 0];
  const Float bow = std::exp2(properties.bend);
  const Float count = static_cast<Float>(SEGMENTS);
  Position from = {slot.x, slot.y};
  for (Whole at = 0; at < SEGMENTS; at += 1) {
    const Float along = static_cast<Float>(at + 1) / count;
    const Position to = {
      slot.x + run * along, slot.y + drop * std::pow(curve(along), bow)};
    elements[at] = ::piece(from, to, thickness, ink);
    from = to;
  }
}
