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
#include <island/gui/backend/native/kinds.snap.hpp>
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

}  // namespace

auto GUI::KINDS::line(
  GUI::Position from, GUI::Position to, Float thickness,
  const GFX::Color &ink) -> GUI::Element {
  const Float length = std::hypot(to.x - from.x, to.y - from.y);
  if (length == 0.0f) return {};
  const Float fringe = BACKEND::NATIVE::GET::pixel();
  const Float width = std::max(thickness, fringe);
  const GUI::Position axis = {
    (to.x - from.x) / length, (to.y - from.y) / length};
  const Float half = width / 2.0f;
  GFX::Color tint = ink;
  tint.a *= thickness / width;
  return {
    .x = from.x + axis.y * half,
    .y = from.y - axis.x * half,
    .w = length,
    .h = width,
    .color = tint,
    .axis = axis,
    .fringe = fringe};
}

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
    elements[at] = line(from, to, thickness, ink);
    from = to;
  }
  elements[0].caps += CAPS::NEAR;
  elements[SEGMENTS - 1].caps += CAPS::FAR;
}
