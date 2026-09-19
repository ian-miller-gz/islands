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
  "graph field x=20 y=20 w=280 h=180\n"
  "  node osc x=10 y=10 w=80 h=40 text=\"osc\"\n"
  "button plate x=180 y=60 w=60 h=20 text=\"plate\"\n";

void at(Float x, Float y) { aim(x * 2.0f, y * 2.0f); }

void door(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  const auto stroke = GUI::NGA::GET::stroked(::bench, "field");
  const auto press = GUI::NGA::GET::pressed(::bench, "field");
  const auto pan = GUI::NGA::GET::pan(::bench, "field");
  std::cout << std::fixed << std::setprecision(1) << "shadow " << name
            << ": press=" << (press.board.empty() ? "-" : press.board)
            << " target=" << (press.target.empty() ? "-" : press.target)
            << " stroke=" << (stroke.board.empty() ? "-" : stroke.board)
            << " pan=" << pan.x << "," << pan.y << std::endl;
}

Status covered() {
  ::at(210.0f, 70.0f);
  hold(true);
  ::door("covered");
  Status status = check(
    "no stroke stands", GUI::NGA::GET::stroked(::bench, "field").board.empty());
  hold(false);
  ::door("uncovered");
  return status |
         check("the claimant clicks", GUI::GET::clicked(::bench, "plate"));
}

Status uncovered() {
  ::at(70.0f, 50.0f);
  hold(true);
  ::door("box");
  Status status = check(
    "the box strokes", !GUI::NGA::GET::stroked(::bench, "field").board.empty());
  hold(false);
  ::door("dropped");
  ::at(260.0f, 160.0f);
  hold(true);
  ::door("ground");
  hold(false);
  ::door("lifted");
  return status;
}

Status dragged() {
  ::at(210.0f, 70.0f);
  grip(true);
  ::door("shadowed grab");
  ::at(170.0f, 70.0f);
  ::door("shadowed drag");
  Status status = check(
    "the view never moved", GUI::NGA::GET::pan(::bench, "field").x == 0.0f);
  grip(false);
  ::door("shadowed let go");
  ::at(260.0f, 160.0f);
  grip(true);
  ::door("open grab");
  ::at(220.0f, 160.0f);
  ::door("open drag");
  status |= check(
    "the open view carried", GUI::NGA::GET::pan(::bench, "field").x != 0.0f);
  grip(false);
  ::door("released");
  return status;
}

}  // namespace

Status shadowed() {
  const String layout = LAYOUT;
  ::bench = GUI::load("shadow", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("shadow load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::at(0.0f, 0.0f);
  hold(false);
  ::door("rest");
  status |= ::covered();
  status |= ::uncovered();
  return status | ::dragged();
}
