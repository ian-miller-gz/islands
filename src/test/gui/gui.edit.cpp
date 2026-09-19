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

GUI::Handle sheet = GUI::NONE;

constexpr STRING::Hot LAYOUT =
  "style entry color=#00000000\n"
  "field name x=8 y=8 w=200 h=24\n"
  "field note x=8 y=40 w=200 h=24\n"
  "list menu x=8 y=72 w=200 h=58 rows=4 plain=entry\n";

Whole written = 0;

constexpr Float LINE = 12.0f;
constexpr Float ADVANCE = 8.0f;

auto name(GUI::Event::Kind kind) -> STRING::Hot {
  static constexpr STRING::Hot WORDS[] = {"entered", "exited",   "pressed",
                                          "clicked", "selected", "activated",
                                          "edited",  "committed"};
  return WORDS[kind];
}

auto marked() -> String {
  for (const auto &node : GUI::STATE::documents[::sheet].nodes)
    if (node.live && node.properties.seated) return node.id;
  return "-";
}

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::sheet, pass);
  const auto caret = GUI::GET::caret(::sheet, "name");
  const auto seat = GUI::GET::editing(::sheet);
  std::cout << "edit " << name << ": name='" << GUI::GET::text(::sheet, "name")
            << "' note='" << GUI::GET::text(::sheet, "note")
            << "' caret=" << caret.at << "," << caret.from
            << " seat=" << (seat.empty() ? "-" : seat)
            << " marks=" << ::marked()
            << " cursor=" << GUI::GET::cursor(::sheet, "menu") << " events=[";
  STRING::Hot comma = "";
  for (const auto &event : GUI::GET::events(::sheet)) {
    std::cout << comma << event.id << " " << ::name(event.kind);
    comma = ", ";
  }
  const auto writes = GUI::GET::writes();
  std::cout << "] rewrites=" << writes - ::written << std::endl;
  ::written = writes;
}

void write(STRING::Hot text) {
  Vector<INPUT::KEYS::Event> events;
  for (STRING::Hot at = text; *at != '\0'; at += 1)
    events.push_back({INPUT::KEYS::TEXT, static_cast<Whole>(*at)});
  typed(events);
}

void press(INPUT::KEYS::Action action, Flag shift = false) {
  typed({{action, 0, false, shift}});
}

void press(
  INPUT::KEYS::Action first, INPUT::KEYS::Action second, Flag shift = false) {
  typed({{first, 0, false, shift}, {second, 0, false, shift}});
}

void at(Float x, Float y, Flag down) {
  aim(x, y);
  hold(down);
}

}  // namespace

Status edit() {
  const String layout = LAYOUT;
  ::sheet = GUI::load("edit", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("edit load", ::sheet != GUI::NONE);
  write("no");
  ::look("deaf");
  status |= check("focus", GUI::focus(::sheet, true) == 0);
  ::press(INPUT::KEYS::DOWN);
  ::look("walks");
  status |= check("seat", GUI::edit(::sheet, "name") == 0);
  status |= check("refuse a non-field seat", GUI::edit(::sheet, "menu") != 0);
  write("path");
  ::look("typed");
  status |= check("edited answers once", GUI::GET::edited(::sheet, "name"));
  status |= check("edited consumed", !GUI::GET::edited(::sheet, "name"));
  ::press(INPUT::KEYS::DOWN);
  ::look("hands");
  status |= check("reseat", GUI::edit(::sheet, "name") == 0);
  ::press(INPUT::KEYS::HOME);
  ::look("home");
  ::press(INPUT::KEYS::RIGHT, INPUT::KEYS::RIGHT, true);
  ::look("select");
  write("R");
  ::look("replace");
  typed({{INPUT::KEYS::END}, {INPUT::KEYS::TEXT, 's'}});
  ::look("append");
  ::press(INPUT::KEYS::ERASE);
  ::look("erase");
  ::press(INPUT::KEYS::HOME, INPUT::KEYS::DELETE);
  ::look("delete");
  ::press(INPUT::KEYS::ENTER);
  ::look("commit");
  status |= check("committed answers once", GUI::GET::committed(sheet, "name"));
  status |= check("committed consumed", !GUI::GET::committed(sheet, "name"));
  typed({{INPUT::KEYS::TEXT, INPUT::KEYS::TAB}});
  ::look("step");
  write("two");
  ::look("second");
  status |= check("reseat for the chords", GUI::edit(::sheet, "name") == 0);
  typed(
    {{INPUT::KEYS::HOME},
     {INPUT::KEYS::RIGHT, 0, false, true},
     {INPUT::KEYS::TEXT, INPUT::BINDINGS::keys["GUI::copy"], true}});
  ::look("copied");
  status |= check("the selection reached the clipboard", clipping() == "t");
  clipped("seed/");
  typed({{INPUT::KEYS::TEXT, INPUT::BINDINGS::keys["GUI::paste"], true}});
  ::look("pasted");
  INPUT::TEXT::claim(true);
  write("lost");
  ::look("claimed");
  status |= check("refocus", GUI::focus(::sheet, true) == 0);
  status |= check("release the seat", GUI::edit(::sheet, "") == 0);
  ::press(INPUT::KEYS::DOWN);
  ::look("returns");
  ::metrics(LINE, ADVANCE);
  status |= check("place", GUI::place(::sheet, WHOLE) == 0);
  status |= check(
    "a measured run", GUI::set(::sheet, "name", GUI::Text{"cartridges/"}) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("measured");
  ::at(44.0f, 20.0f, true);
  ::look("aims");
  ::at(44.0f, 20.0f, false);
  ::look("places");
  ::at(160.0f, 20.0f, true);
  ::look("reaches");
  ::at(160.0f, 20.0f, false);
  ::look("beyond");
  ::at(44.0f, 20.0f, true);
  ::look("grabs");
  ::at(46.0f, 20.0f, true);
  ::look("wobble");
  ::at(84.0f, 20.0f, true);
  ::look("sweeps");
  ::at(140.0f, 20.0f, true);
  ::look("widens");
  ::at(140.0f, 20.0f, false);
  ::look("kept");
  write("X");
  ::look("swapped");
  return status;
}
