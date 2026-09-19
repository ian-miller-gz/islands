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

GUI::Handle bench = GUI::NONE;

constexpr STRING::Hot LAYOUT =
  "graph deck x=20 y=20 w=280 h=180\n"
  "  node osc x=30 y=30 w=80 h=40 text=\"osc\"\n";

void felt(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  const auto stroke = GUI::NGA::GET::stroked(::bench, "deck");
  std::cout << std::fixed << std::setprecision(1) << "stroke " << name << ": ";
  if (stroke.board.empty()) {
    std::cout << "-" << std::endl;
    return;
  }
  std::cout << stroke.board
            << " target=" << (stroke.target.empty() ? "-" : stroke.target)
            << " @" << stroke.x << "," << stroke.y << std::endl;
}

void again(STRING::Hot name) {
  const auto stroke = GUI::NGA::GET::stroked(::bench, "deck");
  std::cout << std::fixed << std::setprecision(1) << "stroke " << name << ": ";
  if (stroke.board.empty()) {
    std::cout << "-" << std::endl;
    return;
  }
  std::cout << stroke.board
            << " target=" << (stroke.target.empty() ? "-" : stroke.target)
            << " @" << stroke.x << "," << stroke.y << std::endl;
}

void at(Float x, Float y, Flag down) {
  aim(x, y);
  hold(down);
}

}  // namespace

Status stroked() {
  const String layout = LAYOUT;
  rescale(2.0f);
  ::bench = GUI::load("stroked", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("stroked load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::at(0.0f, 0.0f, false);
  ::felt("rest");
  ::at(140.0f, 120.0f, true);
  ::felt("press");
  ::again("standing");
  status |= check(
    "pressed drains beside it",
    !GUI::NGA::GET::pressed(::bench, "deck").board.empty());
  ::again("after the drain");
  ::at(300.0f, 300.0f, true);
  ::felt("dragged");
  ::at(300.0f, 300.0f, false);
  ::felt("released");
  status |= check(
    "pan", GUI::NGA::set(::bench, "deck", GUI::NGA::Pan{10.0f, 5.0f}) == 0);
  status |=
    check("zoom", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f}) == 0);
  ::at(520.0f, 360.0f, true);
  ::felt("bare");
  ::at(480.0f, 380.0f, true);
  ::felt("run");
  ::at(480.0f, 380.0f, false);
  ::felt("done");
  status |= check(
    "unknown answers empty",
    GUI::NGA::GET::stroked(::bench, "ghost").board.empty());
  status |=
    check("rehome", GUI::set(::bench, "osc", GUI::Position{30.0f, 30.0f}) == 0);
  ::felt("rehomed");
  status |= check("stage for the arc", GUI::stage(::bench) == 0);
  status |= check("driven", GUI::drag(::bench, "osc", 120.0f, 40.0f, 1) == 0);
  status |= check(
    "nothing stands after the arc",
    GUI::NGA::GET::stroked(::bench, "deck").board.empty());
  const auto press = GUI::NGA::GET::pressed(::bench, "deck");
  const auto lift = GUI::NGA::GET::lifted(::bench, "deck");
  std::cout << std::fixed << std::setprecision(1)
            << "stroke arc: " << (press.target.empty() ? "-" : press.target)
            << " @" << press.x << "," << press.y << " -> @" << lift.x << ","
            << lift.y << std::endl;
  status |= check("the arc opened", !press.board.empty());
  status |= check("the arc closed", !lift.board.empty());
  status |=
    check("closed once", GUI::NGA::GET::lifted(::bench, "deck").board.empty());
  return status;
}
