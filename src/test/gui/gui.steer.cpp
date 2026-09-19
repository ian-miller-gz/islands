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

GUI::Handle roll = GUI::NONE;

constexpr STRING::Hot NAMES[] = {"alpha", "beta", "gamma", "delta", "echo"};
constexpr Whole COUNT = 5;

constexpr STRING::Hot LAYOUT =
  "style entry color=#00000000\n"
  "style selected color=#ffffff40\n"
  "list menu x=8 y=8 w=200 h=58 rows=5 mark=selected plain=entry\n"
  "list back x=8 y=80 w=200 h=58 rows=3 mark=selected plain=entry\n";

Whole written = 0;

auto name(GUI::Event::Kind kind) -> STRING::Hot {
  static constexpr STRING::Hot WORDS[] = {"entered", "exited",   "pressed",
                                          "clicked", "selected", "activated"};
  return WORDS[kind];
}

void look(STRING::Hot name) {
  GUI::poll();
  const auto first = GUI::GET::first(::roll, "menu");
  Whole shown = 0;
  for (Whole at = 0; at < ::COUNT; at += 1) {
    const String id = "menu." + std::to_string(at);
    Whole node = GUI::PARENTLESS;
    if (GUI::find(::roll, id.c_str(), node) != 0) break;
    if (!GUI::GET::visibility(::roll, id.c_str())) continue;
    shown += 1;
    GUI::set(::roll, id.c_str(), GUI::Text{::NAMES[(first + at) % ::COUNT]});
  }
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::roll, pass);
  const auto top = GUI::GET::text(::roll, "menu.0");
  std::cout << "steer " << name << ": cursor=" << GUI::GET::cursor(roll, "menu")
            << " first=" << first << " top=" << (top.empty() ? "-" : top)
            << " shown=" << shown << " back=" << GUI::GET::cursor(roll, "back")
            << " events=[";
  STRING::Hot comma = "";
  for (const auto &event : GUI::GET::events(::roll)) {
    std::cout << comma << event.id << " " << ::name(event.kind);
    comma = ", ";
  }
  const auto writes = GUI::GET::writes();
  std::cout << "] rewrites=" << writes - ::written << std::endl;
  ::written = writes;
}

void press(INPUT::KEYS::Action action) { typed({{action}}); }

void arc(STRING::Hot down, STRING::Hot up) {
  hold(true);
  ::look(down);
  hold(false);
  ::look(up);
}

Status quicken() {
  GUI::set(::roll, "menu", GUI::Rows{5});
  GUI::set(::roll, "menu", GUI::Cursor{0});
  GUI::set(::roll, "menu", GUI::Scroll{0});
  GUI::set(::roll, "menu", GUI::Quick{true});
  GUI::set(::roll, "back", GUI::Cursor{0});
  aim(20.0f, 35.0f);
  ::arc("arm", "quick");
  Status status =
    check("quick click activates", GUI::GET::activated(roll, "menu"));
  aim(20.0f, 100.0f);
  ::arc("reach", "plain");
  status |= check("plain click selects", GUI::GET::selected(::roll, "back"));
  status |= check("plain click no take", !GUI::GET::activated(::roll, "back"));
  aim(20.0f, 35.0f);
  ::arc("revisit", "marked");
  status |= check("quick marked take", GUI::GET::activated(::roll, "menu"));
  status |= check("quick marked no move", !GUI::GET::selected(::roll, "menu"));
  return status;
}

Status folds() {
  GUI::set(::roll, "menu.0", GUI::Fold{GUI::Fold::CLOSED});
  ::look("closed");
  Status status = check(
    "a stated row wears the closed mark",
    GUI::GET::fold(::roll, "menu.0").stand == GUI::Fold::CLOSED);
  status |= check(
    "a row never stated wears none",
    GUI::GET::fold(::roll, "menu.1").stand == GUI::Fold::NONE);
  GUI::set(::roll, "menu.0", GUI::Fold{GUI::Fold::CLOSED});
  ::look("standing");
  GUI::set(::roll, "menu.0", GUI::Fold{GUI::Fold::OPEN});
  ::look("unfolded");
  status |= check(
    "the stand moves to open",
    GUI::GET::fold(::roll, "menu.0").stand == GUI::Fold::OPEN);
  GUI::set(::roll, "menu.0", GUI::Fold{GUI::Fold::NONE});
  ::look("bare");
  return status | check(
                    "the mark comes off again",
                    GUI::GET::fold(::roll, "menu.0").stand == GUI::Fold::NONE);
}

}  // namespace

Status steer() {
  const String layout = LAYOUT;
  ::roll = GUI::load("steer", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("steer load", ::roll != GUI::NONE);
  ::look("bank");
  ::press(INPUT::KEYS::DOWN);
  ::look("deaf");
  status |= check("focus", GUI::focus(::roll, true) == 0);
  ::press(INPUT::KEYS::DOWN);
  ::look("down");
  status |= check("selected answers once", GUI::GET::selected(::roll, "menu"));
  status |= check("selected consumed", !GUI::GET::selected(::roll, "menu"));
  typed({{INPUT::KEYS::DOWN}, {INPUT::KEYS::DOWN}});
  ::look("chase");
  ::press(INPUT::KEYS::END);
  ::look("end");
  ::press(INPUT::KEYS::DOWN);
  ::look("wrap");
  ::press(INPUT::KEYS::UP);
  ::look("rise");
  ::press(INPUT::KEYS::PRIOR);
  ::look("prior");
  ::press(INPUT::KEYS::NEXT);
  ::look("next");
  ::press(INPUT::KEYS::HOME);
  ::look("home");
  ::press(INPUT::KEYS::ENTER);
  ::look("take");
  status |= check("activated answers once", GUI::GET::activated(roll, "menu"));
  status |= check("activated consumed", !GUI::GET::activated(::roll, "menu"));
  status |= check("place", GUI::place(::roll, WHOLE) == 0);
  aim(20.0f, 20.0f);
  ::look("hover");
  wheeled(-1.0f, false);
  ::look("spin");
  wheeled(0.0f, false);
  ::press(INPUT::KEYS::DOWN);
  ::look("return");
  INPUT::TEXT::claim(true);
  ::press(INPUT::KEYS::DOWN);
  ::look("claimed");
  status |= check("refocus", GUI::focus(::roll, true) == 0);
  ::press(INPUT::KEYS::DOWN);
  ::look("recover");
  GUI::set(::roll, "menu", GUI::Visibility{false});
  ::press(INPUT::KEYS::DOWN);
  ::look("second");
  GUI::set(::roll, "menu", GUI::Visibility{true});
  GUI::set(::roll, "menu", GUI::Rows{2});
  ::look("shrink");
  status |= check("release", GUI::focus(::roll, false) == 0);
  ::press(INPUT::KEYS::DOWN);
  ::look("released");
  status |= ::quicken();
  return status | ::folds();
}
