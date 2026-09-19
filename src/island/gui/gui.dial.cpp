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
#include <island/gui/gui.internal.hpp>
#include <algorithm>
#include <cmath>

auto GUI::settle(const Properties &properties, Float value) -> Float {
  if (properties.most <= properties.least) return value;
  const Float held = std::clamp(value, properties.least, properties.most);
  if (properties.steps == 0) return held;
  const Float span = properties.most - properties.least;
  const auto steps = static_cast<Float>(properties.steps);
  const Float landed = std::round((held - properties.least) / span * steps);
  return properties.least + landed / steps * span;
}

auto GUI::set(Handle document, STRING::Hot id, Value value) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  const Float held = settle(properties, value.value);
  if (properties.value == held) return 0;
  properties.value = held;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Dial dial) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  const Flag same =
    properties.least == dial.least && properties.most == dial.most &&
    properties.resting == dial.resting && properties.steps == dial.steps &&
    properties.graphic == dial.graphic;
  properties.least = dial.least;
  properties.most = dial.most;
  properties.resting = dial.resting;
  properties.steps = dial.steps;
  properties.graphic = dial.graphic;
  const Float held = settle(properties, properties.value);
  if (same && properties.value == held) return 0;
  properties.value = held;
  return mark(document, node);
}

auto GUI::GET::value(Handle document, STRING::Hot id) -> Float {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0.0f;
  return STATE::documents[document].nodes[node].properties.value;
}

auto GUI::GET::dial(Handle document, STRING::Hot id) -> Dial {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const auto &properties = STATE::documents[document].nodes[node].properties;
  return {
    properties.least, properties.most, properties.resting, properties.steps,
    properties.graphic};
}
