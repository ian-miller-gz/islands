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
#include <iostream>
#include <test/gui/gui.internal.hpp>

namespace {

GUI::Handle desk = GUI::NONE;
GUI::Handle quiet = GUI::NONE;

constexpr STRING::Hot LAYOUT =
  "style entry color=#00000000\n"
  "list menu x=8 y=8 w=200 h=58 rows=5 plain=entry\n"
  "field name x=8 y=80 w=200 h=24\n";

constexpr STRING::Hot ASIDE = "panel still x=8 y=8 w=40 h=20\n";

auto name(INPUT::KEYS::Action action) -> STRING::Hot {
  static constexpr STRING::Hot WORDS[] = {
    "text", "enter", "erase",  "left",  "right", "up",    "down",
    "home", "end",   "delete", "prior", "next",  "hotkey"};
  return WORDS[action];
}

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::desk, pass);
  const auto keyed = GUI::GET::keyed(::desk);
  const auto seat = GUI::GET::editing(::desk);
  std::cout << "keyed " << name << ": key=" << ::name(keyed.action)
            << " code=" << keyed.codepoint
            << " control=" << (keyed.control ? "true" : "false")
            << " cursor=" << GUI::GET::cursor(::desk, "menu") << " name='"
            << GUI::GET::text(::desk, "name") << "'"
            << " seat=" << (seat.empty() ? "-" : seat)
            << " aside=" << ::name(GUI::GET::keyed(::quiet).action)
            << std::endl;
}

void press(INPUT::KEYS::Action action) { typed({{action}}); }
void letter(Whole codepoint, Flag control) {
  typed({{INPUT::KEYS::TEXT, codepoint, control}});
}

auto empty(GUI::Handle document) -> Flag {
  const auto keyed = GUI::GET::keyed(document);
  return keyed.action == INPUT::KEYS::TEXT && keyed.codepoint == 0;
}

}  // namespace

Status keyed() {
  const String layout = LAYOUT;
  const String aside = ASIDE;
  ::desk = GUI::load("keyed", Vector<Byte>(layout.begin(), layout.end()));
  ::quiet = GUI::load("aside", Vector<Byte>(aside.begin(), aside.end()));
  Status status = check("keyed load", ::desk != GUI::NONE);
  status |= check("aside load", ::quiet != GUI::NONE);
  ::press(INPUT::KEYS::DELETE);
  ::look("deaf");
  status |= check("focus", GUI::focus(::desk, true) == 0);
  ::press(INPUT::KEYS::DELETE);
  ::look("unclaimed");
  status |= check("drained", ::empty(::desk));
  ::press(INPUT::KEYS::DOWN);
  ::look("steered");
  ::press(INPUT::KEYS::ENTER);
  ::look("taken");
  ::letter('q', false);
  ::look("letter");
  ::letter('d', true);
  ::look("chorded");
  status |= check("seat", GUI::edit(::desk, "name") == 0);
  ::letter('q', false);
  ::look("seated");
  status |= check("release", GUI::edit(::desk, "") == 0);
  ::press(INPUT::KEYS::DELETE);
  GUI::poll();
  ::look("stale");
  GUI::set(::desk, "menu", GUI::Rows{0});
  ::press(INPUT::KEYS::DOWN);
  ::look("deafened");
  status |= check("aside untouched", ::empty(::quiet));
  return status | check("release focus", GUI::focus(::desk, false) == 0);
}
