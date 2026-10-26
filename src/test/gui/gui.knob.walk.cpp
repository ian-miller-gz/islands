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
#include <iomanip>
#include <iostream>
#include <test/gui/gui.internal.hpp>

namespace {

constexpr STRING::Hot CELLS[] = {"knob", "notch", "slide", "pan"};
constexpr STRING::Hot KNOBS[] = {"knob", "notch", "pan"};
constexpr Float SCALE = 2.0f;
constexpr Float HALF = 0.5f;
constexpr Float DEGREES = 180.0f / std::numbers::pi_v<Float>;
constexpr Float EPSILON = 0.01f;

auto degrees(Float along) -> Float {
  return (along - ::HALF) * GUI::KINDS::SWEEP * ::DEGREES;
}

void arced(GUI::Handle document, STRING::Hot cell) {
  Whole node = GUI::PARENTLESS;
  GUI::find(document, cell, node);
  const auto &properties =
    GUI::STATE::documents[document].nodes[node].properties;
  const auto arcs = GUI::KINDS::arcs(properties);
  const auto filled = std::ranges::find(arcs, true, &GUI::KINDS::Arc::filled);
  const Float span = properties.most - properties.least;
  const Float value = ::degrees(
    std::clamp((properties.value - properties.least) / span, 0.0f, 1.0f));
  const Float end = ::degrees(filled->to);
  std::cout << "arcs " << arcs[0].pieces << "/" << filled->pieces << "/"
            << arcs.back().pieces << " end " << end;
  if (std::abs(end - value) > ::EPSILON) std::cout << " adrift " << value;
  std::cout << std::endl;
}

auto holder(GUI::Handle document) -> String {
  for (const auto &node : GUI::STATE::documents[document].nodes)
    if (node.live && node.properties.seated) return node.id;
  return "-";
}

}  // namespace

void KNOB::look(GUI::Handle document, STRING::Hot leg) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(document, pass);
  std::cout << "leg " << leg << std::endl << std::fixed << std::setprecision(3);
  for (const auto cell : ::CELLS)
    std::cout << "value " << cell << " " << GUI::GET::value(document, cell)
              << std::endl;
  std::cout << "seated " << ::holder(document) << std::endl;
  for (const auto cell : ::KNOBS) ::arced(document, cell);
}

void KNOB::walk(GUI::Handle document, std::span<const Step> steps) {
  for (const auto &step : steps) {
    aim(step.x * ::SCALE, step.y * ::SCALE);
    hold(step.down);
    look(document, step.leg);
  }
}

auto KNOB::measure(GUI::Handle document) -> Status {
  rescale(::SCALE);
  walk(document, {{{0, 0, false, "rest"}}});
  const auto runs = inked();
  if (runs.size() < 2) return check("reading", false);
  std::cout << std::setprecision(1) << "reading knob " << runs[0].x << " slide "
            << runs[1].x << std::endl;
  return check("reading east", runs[0].x > runs[1].x);
}
