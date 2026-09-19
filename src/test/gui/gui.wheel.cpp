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
  "  list menu x=140 y=10 w=100 h=54 rows=6 pitch=14\n"
  "button away x=8 y=210 w=60 h=20 text=\"away\"\n";

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  const auto box = GUI::NGA::GET::wheel(::bench, "osc");
  const auto turned = GUI::NGA::GET::wheel(::bench, "deck");
  const auto over = GUI::GET::hover(::bench);
  std::cout << std::fixed << std::setprecision(1) << "wheel " << name
            << ": box=" << box.turns << " turns=" << turned.turns
            << " control=" << (turned.control ? "true" : "false")
            << " at=" << turned.across << "," << turned.down
            << " first=" << GUI::GET::first(::bench, "menu")
            << " hover=" << (over.empty() ? "-" : over) << std::endl;
}

void over(Float x, Float y) { aim(x * 2.0f, y * 2.0f); }

void reach(STRING::Hot id) {
  const auto corner = GUI::GET::origin(::bench, id);
  const auto size = GUI::GET::measured(::bench, id);
  ::over(corner.x + size.w / 2.0f, corner.y + size.h / 2.0f);
}

}  // namespace

Status spun() {
  const String layout = LAYOUT;
  rescale(2.0f);
  ::bench = GUI::load("wheel", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("wheel load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::over(0.0f, 0.0f);
  ::look("rest");
  ::over(140.0f, 160.0f);
  wheeled(1.0f, false);
  ::look("turned");
  status |=
    check("drained", GUI::NGA::GET::wheel(::bench, "deck").turns == 0.0f);
  wheeled(-2.0f, true);
  ::look("chorded");
  status |=
    check("view untouched", GUI::NGA::GET::zoom(::bench, "deck").value == 1.0f);
  status |=
    check("pan untouched", GUI::NGA::GET::pan(::bench, "deck").x == 0.0f);
  ::reach("osc");
  wheeled(1.0f, false);
  ::look("over a box");
  status |= check(
    "pan", GUI::NGA::set(::bench, "deck", GUI::NGA::Pan{20.0f, 10.0f}) == 0);
  status |=
    check("zoom", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f}) == 0);
  wheeled(0.0f, false);
  ::look("viewed");
  ::reach("osc");
  wheeled(1.0f, true);
  ::look("under the view");
  status |= check(
    "restored", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{1.0f}) == 0);
  status |= check(
    "unpanned", GUI::NGA::set(::bench, "deck", GUI::NGA::Pan{0.0f, 0.0f}) == 0);
  wheeled(0.0f, false);
  ::look("settled");
  ::over(210.0f, 37.0f);
  wheeled(-1.0f, false);
  ::look("over a row");
  ::reach("away");
  wheeled(1.0f, true);
  ::look("off the board");
  wheeled(0.0f, false);
  ::look("still");
  status |= check(
    "wheel refuses a stranger", GUI::wheel(::bench, "ghost", 1.0f, false) != 0);
  status |= check("stage", GUI::stage(::bench) == 0);
  status |= check("driven", GUI::wheel(::bench, "osc", 3.0f, true) == 0);
  ::look("driven");
  status |= check("stage again", GUI::stage(::bench) == 0);
  status |=
    check("driven row", GUI::wheel(::bench, "menu.0", 1.0f, false) == 0);
  ::look("driven row");
  return status;
}
