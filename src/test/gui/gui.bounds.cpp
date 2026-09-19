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
#include <sstream>
#include <test/gui/gui.internal.hpp>

namespace {

GUI::Handle bench = GUI::NONE;

constexpr STRING::Hot LAYOUT =
  "graph deck x=20 y=20 w=280 h=180\n"
  "  node osc x=10 y=10 w=80 h=40 text=\"osc\"\n"
  "graph sheet anchor=stretch x=0 y=200 w=0 h=0\n";

constexpr Float WEST = 0.0f;
constexpr Float EAST = 400.0f;
constexpr Float NORTH = 0.0f;
constexpr Float REACH = 120.0f;

auto wall(Float value) -> String {
  if (value == GUI::NGA::Bounds::NONE) return "open";
  std::ostringstream text;
  text << std::fixed << std::setprecision(1) << value;
  return text.str();
}

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  const auto pan = GUI::NGA::GET::pan(::bench, "deck");
  const auto walls = GUI::NGA::GET::bounds(::bench, "deck");
  std::cout << std::fixed << std::setprecision(1) << "bounds " << name
            << ": pan=" << pan.x << "," << pan.y
            << " across=" << ::wall(walls.west) << ".." << ::wall(walls.east)
            << " down=" << ::wall(walls.north) << ".." << ::wall(walls.south)
            << " zoom=" << GUI::NGA::GET::zoom(::bench, "deck").value
            << std::endl;
}

void at(Float x, Float y) { aim(x * 2.0f, y * 2.0f); }

void put(Float x, Float y) {
  GUI::NGA::set(::bench, "deck", GUI::NGA::Pan{x, y});
}

auto here() -> Float { return GUI::NGA::GET::pan(::bench, "deck").x; }

Status declare() {
  ::look("undeclared");
  ::put(-50.0f, -30.0f);
  ::look("free");
  Status status = check("free", ::here() == -50.0f);
  status |= check(
    "declare",
    GUI::NGA::set(::bench, "deck", GUI::NGA::Bounds{::WEST, ::EAST, ::NORTH}) ==
      0);
  ::look("declared");
  status |= check("snapped in", ::here() == ::WEST);
  ::put(999.0f, 5000.0f);
  ::look("walled");
  status |= check("stopped", ::here() == ::REACH);
  status |=
    check("open below", GUI::NGA::GET::pan(::bench, "deck").y == 5000.0f);
  ::put(30.0f, -99.0f);
  ::look("floored");
  status |=
    check("on the floor", GUI::NGA::GET::pan(::bench, "deck").y == 0.0f);
  return status;
}

Status drag() {
  ::at(250.0f, 150.0f);
  grip(true);
  ::look("grabbed");
  ::at(190.0f, 150.0f);
  ::look("dragging");
  Status status = check("carried", ::here() == 90.0f);
  ::at(70.0f, 150.0f);
  ::look("against the wall");
  status |= check("held at the wall", ::here() == ::REACH);
  ::at(110.0f, 150.0f);
  ::look("pulled back");
  status |= check("no banked slack", ::here() == 80.0f);
  grip(false);
  ::at(110.0f, 150.0f);
  ::look("let go");
  status |= check("no snap back", ::here() == 80.0f);
  return status;
}

Status magnify() {
  ::at(280.0f, 150.0f);
  wheeled(1.0f, true);
  ::look("turned");
  const auto turned = GUI::NGA::GET::wheel(::bench, "deck");
  ::put(turned.across, turned.down);
  ::look("re-centred");
  Status status = check("re-centre walled", ::here() == ::REACH);
  wheeled(0.0f, false);
  status |= check(
    "zoom out", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{0.5f}) == 0);
  ::look("zoomed out");
  status |= check("parked", ::here() == ::WEST);
  status |=
    check("zoom in", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f}) == 0);
  ::put(999.0f, 0.0f);
  ::look("zoomed in");
  status |= check("further", ::here() == 260.0f);
  status |= check(
    "zoom back", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{1.0f}) == 0);
  ::look("zoom restored");
  status |= check("settled again", ::here() == ::REACH);
  return status;
}

}  // namespace

Status walled() {
  const String layout = LAYOUT;
  rescale(2.0f);
  ::bench = GUI::load("bounds", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("bounds load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::at(0.0f, 0.0f);
  status |= ::declare();
  status |= ::drag();
  status |= ::magnify();
  status |= check(
    "open again", GUI::NGA::set(::bench, "deck", GUI::NGA::Bounds{}) == 0);
  ::look("unwalled");
  status |= check("stayed", ::here() == ::REACH);
  ::put(-400.0f, -400.0f);
  ::look("free again");
  status |= check(
    "refuses a stranger",
    GUI::NGA::set(::bench, "ghost", GUI::NGA::Bounds{}) != 0);
  status |= check(
    "a stranger's walls are open",
    GUI::NGA::GET::bounds(::bench, "ghost").west == GUI::NGA::Bounds::NONE);
  return status | stretched(::bench);
}
