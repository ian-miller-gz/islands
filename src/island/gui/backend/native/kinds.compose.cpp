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

auto GUI::KINDS::washed(const GUI::Properties &properties, GFX::Color fill)
  -> GFX::Color {
  if (!properties.selected) return fill;
  const auto &wash = properties.wash;
  const Float keep = 1.0f - wash.a;
  return {
    fill.r * keep + wash.r * wash.a, fill.g * keep + wash.g * wash.a,
    fill.b * keep + wash.b * wash.a, fill.a};
}

void GUI::KINDS::box(
  const GUI::Slot &slot, const GUI::Properties &properties,
  const GFX::Color &fill, GUI::Element *elements) {
  const auto rim = properties.border;
  const auto &edge = properties.edge;
  elements[0] = {slot.x, slot.y,    slot.w,
                 slot.h, GUI::FULL, washed(properties, fill)};
  elements[1] = {slot.x, slot.y, slot.w, rim, GUI::FULL, edge};
  elements[2] = {slot.x, slot.y + slot.h - rim, slot.w, rim, GUI::FULL, edge};
  elements[3] = {slot.x, slot.y + rim, rim, slot.h - 2 * rim, GUI::FULL, edge};
  elements[4] = {slot.x + slot.w - rim, slot.y + rim, rim,
                 slot.h - 2 * rim,      GUI::FULL,    edge};
}

void GUI::KINDS::panel(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  box(slot, node.properties, node.properties.color, elements);
}

void GUI::KINDS::button(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  const GFX::Color fills[] = {
    node.properties.color, node.properties.hover, node.properties.press};
  box(slot, node.properties, fills[node.state], elements);
}

void GUI::KINDS::image(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  elements[0] = {
    slot.x,
    slot.y,
    slot.w,
    slot.h,
    node.properties.frame,
    washed(node.properties, node.properties.color)};
}

void GUI::KINDS::bar(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  elements[0] = {slot.x, slot.y,    slot.w,
                 slot.h, GUI::FULL, node.properties.edge};
  elements[1] = {slot.x, slot.y,    slot.w * node.properties.fraction,
                 slot.h, GUI::FULL, node.properties.color};
}

void GUI::KINDS::marks(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  constexpr Float STROKE = 1.0f;
  const auto &properties = node.properties;
  const Float scale = BACKEND::NATIVE::lettered(node, slot);
  const Float advance = GFX::TEXT::GET::advance(scale);
  const Float step = GFX::TEXT::GET::line(scale);
  const Float x = slot.x + properties.pad * slot.scale.w;
  const Float y = slot.y + (slot.h - step) / 2;
  const Whole first = properties.caret < properties.selection
                        ? properties.caret
                        : properties.selection;
  const Whole last = properties.caret < properties.selection
                       ? properties.selection
                       : properties.caret;
  const Float band =
    properties.seated ? advance * static_cast<Float>(last - first) : 0.0f;
  const Float stroke = properties.seated ? STROKE : 0.0f;
  elements[0] = {
    x + advance * static_cast<Float>(first),
    y,
    band,
    step,
    GUI::FULL,
    properties.hover};
  elements[1] = {
    x + advance * static_cast<Float>(properties.caret),
    y,
    stroke,
    step,
    GUI::FULL,
    properties.edge};
}

void GUI::KINDS::field(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  box(slot, node.properties, node.properties.color, elements);
  marks(node, slot, elements + 5);
}

void GUI::KINDS::dial(
  const GUI::Node &node, const GUI::Slot &slot, GUI::Element *elements) {
  constexpr Float DONGLE = 3.0f;
  const auto &properties = node.properties;
  box(slot, properties, properties.color, elements);
  const Float rim = properties.border;
  const Float x = slot.x + rim;
  const Float y = slot.y + rim;
  const Float width = slot.w - 2 * rim;
  const Float height = slot.h - 2 * rim;
  Float from = x;
  Float span = 0.0f;
  if (properties.graphic && properties.most > properties.least) {
    const Float across = (properties.value - properties.least) /
                         (properties.most - properties.least);
    span = width * std::clamp(across, 0.0f, 1.0f);
  } else if (properties.graphic) {
    from = x + (width - DONGLE) / 2;
    span = DONGLE;
  }
  elements[5] = {from, y, span, height, GUI::FULL, properties.press};
  marks(node, slot, elements + 6);
}
