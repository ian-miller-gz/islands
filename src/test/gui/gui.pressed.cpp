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
  "  node osc x=10 y=10 w=80 h=40 text=\"osc\"\n"
  "button away x=8 y=210 w=60 h=20 text=\"away\"\n";

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  const auto box = GUI::NGA::GET::pressed(::bench, "osc");
  const auto press = GUI::NGA::GET::pressed(::bench, "deck");
  std::cout << std::fixed << std::setprecision(1) << "pressed " << name
            << ": box='" << box.board << "' board='" << press.board
            << "' target='" << press.target << "' at=" << press.x << ","
            << press.y << std::endl;
}

void at(Float x, Float y, Flag down) {
  aim(x * 2.0f, y * 2.0f);
  hold(down);
}

void reach(STRING::Hot id, Flag down) {
  const auto corner = GUI::GET::origin(::bench, id);
  const auto size = GUI::GET::measured(::bench, id);
  ::at(corner.x + size.w / 2.0f, corner.y + size.h / 2.0f, down);
}

}  // namespace

Status pressed() {
  const String layout = LAYOUT;
  rescale(2.0f);
  ::bench = GUI::load("pressed", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("pressed load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("rest");
  ::at(140.0f, 160.0f, true);
  ::look("ground");
  status |=
    check("drained", GUI::NGA::GET::pressed(::bench, "deck").board.empty());
  ::at(140.0f, 160.0f, false);
  ::look("released");
  ::reach("osc", true);
  ::look("box");
  status |=
    check("position untouched", GUI::GET::position(::bench, "osc").x == 10.0f);
  status |=
    check("pan untouched", GUI::NGA::GET::pan(::bench, "deck").x == 0.0f);
  ::reach("osc", false);
  status |= check(
    "pan", GUI::NGA::set(::bench, "deck", GUI::NGA::Pan{20.0f, 10.0f}) == 0);
  status |=
    check("zoom", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f}) == 0);
  ::look("viewed");
  ::reach("osc", true);
  ::look("under the view");
  ::reach("osc", false);
  status |= check(
    "restored", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{1.0f}) == 0);
  status |= check(
    "unpanned", GUI::NGA::set(::bench, "deck", GUI::NGA::Pan{0.0f, 0.0f}) == 0);
  ::look("settled");
  ::reach("away", true);
  ::look("off the board");
  ::reach("away", false);
  status |= check("stage", GUI::stage(::bench) == 0);
  status |= check("driven", GUI::click(::bench, "osc") == 0);
  ::look("driven");
  status |= check("stage again", GUI::stage(::bench) == 0);
  status |= check("driven ground", GUI::click(::bench, "deck") == 0);
  ::look("driven ground");
  status |=
    check("click refuses a stranger", GUI::click(::bench, "ghost") != 0);
  return status;
}
