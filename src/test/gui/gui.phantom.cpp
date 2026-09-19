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
  "button under x=30 y=200 w=80 h=32 text=\"under\"\n"
  "graph deck x=20 y=20 w=280 h=180\n"
  "  node roam x=10 y=10 w=80 h=40 text=\"roam\"\n";

void where(STRING::Hot id) {
  const auto held = GUI::GET::position(::bench, id);
  const auto landed = GUI::GET::origin(::bench, id);
  const auto size = GUI::GET::measured(::bench, id);
  std::cout << " " << id << "@" << held.x << "," << held.y << " +" << landed.x
            << "," << landed.y << " x" << size.w << "," << size.h;
}

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  const auto over = GUI::GET::hover(::bench);
  std::cout << std::fixed << std::setprecision(1) << "phantom " << name << ":";
  ::where("under");
  ::where("roam");
  std::cout << " hover=" << (over.empty() ? "-" : over) << std::endl;
}

}  // namespace

Status phantom() {
  const String layout = LAYOUT;
  rescale(2.0f);
  ::bench = GUI::load("phantom", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("phantom load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  aim(0.0f, 0.0f);
  hold(false);
  ::look("rest");
  aim(120.0f, 80.0f);
  ::look("shown");
  status |= check("shown child answers", GUI::GET::hover(::bench) == "roam");
  status |= check(
    "pan out",
    GUI::NGA::set(::bench, "deck", GUI::NGA::Pan{0.0f, -170.0f}) == 0);
  aim(120.0f, 420.0f);
  ::look("panned out");
  status |=
    check("clipped away answers no press", GUI::GET::hover(::bench) == "under");
  hold(true);
  ::look("press");
  hold(false);
  ::look("release");
  status |= check("beneath clicks", GUI::GET::clicked(::bench, "under"));
  status |= check("phantom never clicks", !GUI::GET::clicked(::bench, "roam"));
  status |= check(
    "pan home", GUI::NGA::set(::bench, "deck", GUI::NGA::Pan{0.0f, 0.0f}) == 0);
  ::look("panned home");
  status |= check("beneath still answers", GUI::GET::hover(::bench) == "under");
  aim(120.0f, 80.0f);
  ::look("home");
  status |= check("child answers again", GUI::GET::hover(::bench) == "roam");
  return status;
}
