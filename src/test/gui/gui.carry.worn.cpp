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

#include <island/window/backend.hpp>
#include <test/gui/gui.internal.hpp>

namespace {

GUI::Handle plate = GUI::NONE;

constexpr STRING::Hot LAYOUT =
  "panel pane x=0 y=0 w=200 h=120\n"
  "  grip rider x=20 y=20 w=60 h=30 axis=free carry=pointer\n"
  "  grip drawn x=20 y=70 w=60 h=30 axis=free\n"
  "  grip whole x=100 y=20 w=60 h=12 axis=free carry=pointer\n"
  "    panel band x=0 y=12 w=80 h=40\n"
  "      panel hidden x=0 y=0 w=180 h=180 visible=false\n";

void look(STRING::Hot name, STRING::Hot id) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::plate, pass);
  const auto at = GUI::GET::position(::plate, id);
  const auto picture = worn();
  std::cout << std::fixed << std::setprecision(1) << "worn " << name << ": "
            << id << "@" << at.x << "," << at.y
            << " shown=" << (GUI::GET::visibility(::plate, id) ? "yes" : "no")
            << " picture=" << picture.w << "x" << picture.h
            << " hot=" << picture.x << "," << picture.y << std::endl;
}

void at(Float x, Float y, Flag down) {
  aim(x, y);
  hold(down);
}

constexpr Float HALF = 0.5f;

auto middle(STRING::Hot id) -> GUI::Position {
  const auto span = GUI::GET::measured(::plate, id);
  return {span.w * HALF, span.h * HALF};
}

constexpr GUI::Position LEG = {20.0f, 20.0f};

Status gesture(const String &label, STRING::Hot id) {
  ::at(0.0f, 0.0f, false);
  ::look((label + " rest").c_str(), id);
  const auto origin = GUI::GET::origin(::plate, id);
  const auto press = ::middle(id);
  const Float x = origin.x + press.x;
  const Float y = origin.y + press.y;
  ::at(x, y, true);
  ::look((label + " press").c_str(), id);
  Status status =
    check((label + " bare press wears nothing").c_str(), !WINDOW::GET::worn());
  ::at(x + LEG.x, y + LEG.y, true);
  ::look((label + " cross").c_str(), id);
  ::at(x + LEG.x * 2, y + LEG.y * 2, true);
  ::look((label + " ride").c_str(), id);
  ::at(x + LEG.x * 2, y + LEG.y * 2, false);
  ::look((label + " moor").c_str(), id);
  status |=
    check((label + " release bares the pointer").c_str(), !WINDOW::GET::worn());
  return status |
         check(
           (label + " drawn again").c_str(), GUI::GET::visibility(::plate, id));
}

}  // namespace

Status ridden() {
  const String layout = LAYOUT;
  rescale(1.0f);
  ::plate = GUI::load("worn", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("plate load", ::plate != GUI::NONE);
  status |=
    check("the word reads back", GUI::GET::carry(::plate, "rider").pointer);
  status |=
    check("its absence reads back", !GUI::GET::carry(::plate, "drawn").pointer);
  status |= check("place", GUI::place(::plate, WHOLE) == 0);
  status |= ::gesture("rider", "rider");
  status |= ::gesture("whole", "whole");
  status |= ::gesture("frame", "drawn");
  status |=
    check("the write lands", GUI::set(::plate, "drawn", GUI::Carry{true}) == 0);
  return status | ::gesture("written", "drawn");
}
