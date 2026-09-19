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
#include <iomanip>
#include <iostream>
#include <test/gui/gui.internal.hpp>

namespace {

constexpr STRING::Hot SHEET = "sheet";
constexpr Float EAST = 400.0f;
constexpr Float REACH = 80.0f;
constexpr GFX::Viewport WIDER = {0.0f, 0.0f, 800.0f, 480.0f};

void at(Float x, Float y) { aim(x * 2.0f, y * 2.0f); }

void look(GUI::Handle bench, STRING::Hot name, const GFX::Viewport &viewport) {
  GUI::poll();
  GFX::Pass pass = {.viewport = viewport};
  GUI::flush(bench, pass);
  const auto pan = GUI::NGA::GET::pan(bench, SHEET);
  const auto size = GUI::GET::measured(bench, SHEET);
  std::cout << std::fixed << std::setprecision(1) << "stretch " << name
            << ": pan=" << pan.x << "," << pan.y << " measured=" << size.w
            << "x" << size.h << std::endl;
}

auto here(GUI::Handle bench) -> Float {
  return GUI::NGA::GET::pan(bench, SHEET).x;
}

Status carried(GUI::Handle bench) {
  ::at(250.0f, 220.0f);
  grip(true);
  ::look(bench, "grabbed", WHOLE);
  ::at(200.0f, 220.0f);
  ::look(bench, "dragging", WHOLE);
  Status status = check("carried", ::here(bench) == 50.0f);
  ::at(120.0f, 220.0f);
  ::look(bench, "against the wall", WHOLE);
  status |= check("held at the measured wall", ::here(bench) == ::REACH);
  ::at(160.0f, 220.0f);
  ::look(bench, "pulled back", WHOLE);
  status |= check("no banked slack", ::here(bench) == 40.0f);
  grip(false);
  return status;
}

Status reshaped(GUI::Handle bench) {
  GUI::NGA::set(bench, SHEET, GUI::NGA::Pan{999.0f, 0.0f});
  ::look(bench, "parked", WHOLE);
  Status status = check("parked at the wall", ::here(bench) == ::REACH);
  ::look(bench, "reshaped", WIDER);
  status |= check("settled by the resize", ::here(bench) == 0.0f);
  ::look(bench, "restored", WHOLE);
  status |= check("still inside", ::here(bench) == 0.0f);
  return status;
}

}  // namespace

Status stretched(GUI::Handle bench) {
  ::look(bench, "authored nothing", WHOLE);
  Status status = check(
    "measured",
    GUI::GET::measured(bench, ::SHEET) == GUI::Extent{320.0f, 40.0f});
  status |= check(
    "wall the sheet",
    GUI::NGA::set(bench, ::SHEET, GUI::NGA::Bounds{0.0f, ::EAST}) == 0);
  ::look(bench, "declared", WHOLE);
  status |= ::carried(bench);
  return status | ::reshaped(bench);
}
