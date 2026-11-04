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

namespace {

struct Samples {
  GUI::Position at[GUI::KINDS::SEGMENTS + 1];
  Float along[GUI::KINDS::SEGMENTS + 1];
};

auto sampled(const GUI::Node &node, const GUI::Slot &slot) -> Samples {
  const auto &properties = node.properties;
  const Float run = properties.to.x * slot.scale.w;
  const Float drop = properties.to.y * slot.scale.h;
  const auto curve =
    ::CURVES[properties.shape < ::SHAPES ? properties.shape : 0];
  const Float bow = std::exp2(properties.bend);
  const Float count = static_cast<Float>(GUI::KINDS::SEGMENTS);
  Samples samples;
  samples.at[0] = {slot.x, slot.y};
  samples.along[0] = 0.0f;
  for (Whole at = 1; at <= GUI::KINDS::SEGMENTS; at += 1) {
    const Float share = static_cast<Float>(at) / count;
    samples.at[at] = {
      slot.x + run * share, slot.y + drop * std::pow(curve(share), bow)};
    samples.along[at] =
      samples.along[at - 1] + std::hypot(
                                samples.at[at].x - samples.at[at - 1].x,
                                samples.at[at].y - samples.at[at - 1].y);
  }
  return samples;
}

auto reached(const Samples &samples, Float walked) -> GUI::Position {
  for (Whole at = 1; at <= GUI::KINDS::SEGMENTS; at += 1) {
    if (walked > samples.along[at] && at < GUI::KINDS::SEGMENTS) continue;
    const Float span = samples.along[at] - samples.along[at - 1];
    const Float share =
      span > 0.0f ? (walked - samples.along[at - 1]) / span : 0.0f;
    const auto &near = samples.at[at - 1];
    const auto &far = samples.at[at];
    return {
      near.x + (far.x - near.x) * share, near.y + (far.y - near.y) * share};
  }
  return samples.at[GUI::KINDS::SEGMENTS];
}

void dashed(
  const GUI::Node &node, const GUI::Slot &slot, Float thickness,
  const GFX::Color &ink, GUI::Element *elements) {
  const auto samples = ::sampled(node, slot);
  const Float length = samples.along[GUI::KINDS::SEGMENTS];
  const Float stated = node.properties.dash * slot.scale.w;
  const Float dash =
    std::max(stated, length / (2.0f * static_cast<Float>(GUI::KINDS::DASHES)));
  Whole last = 0;
  for (Whole at = 0; at < GUI::KINDS::DASHES; at += 1) {
    const Float start = static_cast<Float>(at) * 2.0f * dash;
    if (start >= length || dash <= 0.0f) {
      elements[at] = {};
      continue;
    }
    elements[at] = GUI::KINDS::line(
      ::reached(samples, start),
      ::reached(samples, std::min(start + dash, length)), thickness, ink);
    last = at;
  }
  elements[0].caps += GUI::CAPS::NEAR;
  elements[last].caps += GUI::CAPS::FAR;
}

}  // namespace

void GUI::KINDS::stroke(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  const auto &properties = node.properties;
  const Float thickness = properties.border * slot.scale.w;
  const auto ink = washed(properties, properties.color);
  if (properties.dash > 0.0f)
    return ::dashed(node, slot, thickness, ink, elements);
  const auto samples = ::sampled(node, slot);
  for (Whole at = 0; at < SEGMENTS; at += 1)
    elements[at] = line(samples.at[at], samples.at[at + 1], thickness, ink);
  elements[0].caps += CAPS::NEAR;
  elements[SEGMENTS - 1].caps += CAPS::FAR;
}
