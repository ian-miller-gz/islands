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
  "  node env x=150 y=90 w=80 h=40 text=\"env\"\n"
  "button away x=8 y=210 w=60 h=20 text=\"away\"\n";

constexpr Float SCALE = 2.0f;
constexpr Float MIDDLE = 0.5f;

void look(STRING::Hot name, Flag polled = true) {
  if (polled) {
    GUI::poll();
    GFX::Pass pass = {.viewport = WHOLE};
    GUI::flush(::bench, pass);
  }
  const auto rest = GUI::NGA::GET::hovered(::bench, "deck");
  const auto over = GUI::GET::hover(::bench);
  std::cout << std::fixed << std::setprecision(1) << "hovered " << name
            << ": board='" << rest.board << "' target='" << rest.target
            << "' at=" << rest.x << "," << rest.y
            << " hover=" << (over.empty() ? "-" : over) << std::endl;
}

void at(Float x, Float y, Flag down) {
  aim(x * ::SCALE, y * ::SCALE);
  hold(down);
}

void reach(STRING::Hot id, Flag down) {
  const auto corner = GUI::GET::origin(::bench, id);
  const auto size = GUI::GET::measured(::bench, id);
  ::at(corner.x + size.w * ::MIDDLE, corner.y + size.h * ::MIDDLE, down);
}

auto claimed() -> Status {
  const String layout = "panel cover x=0 y=0 w=10 h=10\n";
  const auto cover =
    GUI::load("cover", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("claim", GUI::claim(cover, true) == 0);
  ::look("claimed away");
  status |= check("unclaim", GUI::claim(cover, false) == 0);
  ::look("unclaimed");
  return status;
}

auto driven() -> Status {
  ::reach("away", false);
  Status status = check("hover", GUI::hover(::bench, "osc") == 0);
  ::look("driven");
  ::look("driven rests");
  status |= check("offset", GUI::hover(::bench, "deck", 0.0f, 60.0f) == 0);
  ::look("driven offset");
  ::at(0.0f, 0.0f, true);
  ::look("stirred");
  ::at(0.0f, 0.0f, false);
  status |= check("stranger refused", GUI::hover(::bench, "ghost") != 0);
  status |= check("press", GUI::press(::bench, "env", 0.0f, 0.0f) == 0);
  status |= check("press refused", GUI::hover(::bench, "osc") != 0);
  status |= check("release", GUI::release(::bench, 0.0f, 0.0f) == 0);
  return status;
}

}  // namespace

Status hovered() {
  const String layout = LAYOUT;
  rescale(::SCALE);
  ::bench = GUI::load("hovered", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("hovered load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("off the board");
  ::reach("osc", false);
  ::look("osc");
  ::look("osc again", false);
  ::reach("env", false);
  ::look("env");
  ::at(200.0f, 50.0f, false);
  ::look("bare");
  ::reach("away", false);
  ::look("away");
  ::reach("osc", true);
  ::look("pressed");
  ::reach("osc", false);
  ::look("released");
  status |= ::claimed();
  status |= driven();
  status |=
    check("unknown", GUI::NGA::GET::hovered(::bench, "ghost").board.empty());
  return status;
}
