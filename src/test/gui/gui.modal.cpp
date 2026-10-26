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
  "button under x=20 y=20 w=80 h=30 text=\"under\"\n"
  "graph deck x=20 y=80 w=120 h=80\n"
  "panel over x=0 y=0 w=320 h=200 z=1 modal=true\n"
  "  button inside x=200 y=150 w=80 h=30 text=\"inside\"\n";

constexpr Float SCALE = 2.0f;
constexpr Float MIDDLE = 0.5f;
constexpr Float NOTCH = 1.0f;

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  const auto over = GUI::GET::hover(::bench);
  const auto turned = GUI::NGA::GET::wheel(::bench, "deck");
  std::cout << std::fixed << std::setprecision(1) << "modal " << name
            << ": hover=" << (over.empty() ? "-" : over)
            << " turns=" << turned.turns << std::endl;
}

void reach(STRING::Hot id) {
  const auto corner = GUI::GET::origin(::bench, id);
  const auto size = GUI::GET::measured(::bench, id);
  aim(
    (corner.x + size.w * ::MIDDLE) * ::SCALE,
    (corner.y + size.h * ::MIDDLE) * ::SCALE);
  hold(false);
}

void click(STRING::Hot id) {
  GUI::stage(::bench);
  const auto refused = GUI::click(::bench, id) != 0;
  const auto clicked = GUI::GET::clicked(::bench, id);
  std::cout << "modal click " << id << ": "
            << (refused   ? "refused"
                : clicked ? "clicked"
                          : "missed")
            << std::endl;
}

void roll(STRING::Hot name) {
  wheeled(::NOTCH, false);
  ::reach("deck");
  ::look(name);
  wheeled(0.0f, false);
}

}  // namespace

Status veiling() {
  const String layout = LAYOUT;
  rescale(::SCALE);
  ::bench = GUI::load("modal", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("modal load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::look("rest");
  ::reach("under");
  ::look("under");
  ::reach("inside");
  ::look("inside");
  ::roll("deck");
  ::click("under");
  ::click("inside");
  status |=
    check("hide", GUI::set(::bench, "over", GUI::Visibility{false}) == 0);
  ::look("hidden");
  ::reach("under");
  ::look("under");
  ::roll("deck");
  ::click("under");
  return status;
}
