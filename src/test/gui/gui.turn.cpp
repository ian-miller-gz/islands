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
  "field name x=8 y=8 w=100 h=20\n"
  "dial gain x=8 y=40 w=200 h=20 least=0 most=1 resting=0.3 value=0.3\n"
  "dial free x=8 y=70 w=200 h=20 value=440\n"
  "dial mode x=8 y=100 w=200 h=20 least=0 most=1 steps=1 resting=0 "
  "graphic=true\n"
  "list menu x=8 y=130 w=200 h=58 rows=4\n"
  "dial slide x=8 y=200 w=200 h=20 least=0 most=1 resting=0.5 graphic=true "
  "value=0.5\n";

Whole written = 0;

constexpr Float LINE = 12.0f;
constexpr Float ADVANCE = 8.0f;

auto name(GUI::Event::Kind kind) -> STRING::Hot {
  static constexpr STRING::Hot WORDS[] = {
    "entered",   "exited", "pressed",   "clicked", "selected",
    "activated", "edited", "committed", "dialled", "moved"};
  return WORDS[kind];
}

auto marked() -> String {
  for (const auto &node : GUI::STATE::documents[::bench].nodes)
    if (node.live && node.properties.seated) return node.id;
  return "-";
}

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  const auto seat = GUI::GET::editing(::bench);
  const auto caret = GUI::GET::caret(::bench, "gain");
  std::cout << std::fixed << std::setprecision(3) << "turn " << name
            << ": gain=" << GUI::GET::value(::bench, "gain")
            << " free=" << GUI::GET::value(::bench, "free")
            << " mode=" << GUI::GET::value(::bench, "mode")
            << " slide=" << GUI::GET::value(::bench, "slide") << " run='"
            << GUI::GET::text(::bench, "gain") << "' caret=" << caret.at << ","
            << caret.from << " seat=" << (seat.empty() ? "-" : seat)
            << " marks=" << ::marked()
            << " cursor=" << GUI::GET::cursor(::bench, "menu") << " events=[";
  STRING::Hot comma = "";
  for (const auto &event : GUI::GET::events(::bench)) {
    std::cout << comma << event.id << " " << ::name(event.kind);
    comma = ", ";
  }
  const auto writes = GUI::GET::writes();
  std::cout << "] rewrites=" << writes - ::written << std::endl;
  ::written = writes;
}

void at(Float x, Float y, Flag down) {
  aim(x, y);
  hold(down);
}

void press(INPUT::KEYS::Action action) { typed({{action, 0, false, false}}); }

}  // namespace

Status turn() {
  const String layout = LAYOUT;
  rescale(2.0f);
  ::bench = GUI::load("turn", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("turn load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  const auto gain = GUI::GET::dial(::bench, "gain");
  status |= check("range", gain.least == 0.0f && gain.most == 1.0f);
  status |= check("resting", gain.resting == 0.3f);
  status |= check("graphic", GUI::GET::dial(::bench, "slide").graphic);
  status |=
    check("write over", GUI::set(::bench, "gain", GUI::Value{9.0f}) == 0);
  status |= check("clamped", GUI::GET::value(::bench, "gain") == 1.0f);
  status |=
    check("write under", GUI::set(::bench, "mode", GUI::Value{0.6f}) == 0);
  status |= check("snapped", GUI::GET::value(::bench, "mode") == 1.0f);
  status |= check("restate", GUI::set(::bench, "gain", GUI::Value{0.3f}) == 0);
  status |= check(
    "boundless keeps its own", GUI::GET::value(::bench, "free") == 440.0f);
  status |=
    check("unsnapped mode", GUI::set(::bench, "mode", GUI::Value{0.0f}) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("rest");
  ::at(40.0f, 100.0f, true);
  ::look("press");
  ::at(44.0f, 100.0f, true);
  ::look("wobble");
  ::at(44.0f, 100.0f, false);
  ::look("click");
  status |= check("close the cell", GUI::edit(::bench, "") == 0);
  ::at(40.0f, 100.0f, true);
  ::look("grab");
  ::at(216.0f, 100.0f, true);
  ::look("drag");
  ::at(416.0f, 100.0f, true);
  ::look("run");
  status |= check("dialled answers once", GUI::GET::dialled(::bench, "gain"));
  status |= check("dialled consumed", !GUI::GET::dialled(::bench, "gain"));
  ::at(416.0f, 100.0f, false);
  ::look("drop");
  ::at(200.0f, 160.0f, true);
  ::look("hold");
  ::at(220.0f, 160.0f, true);
  ::look("creep");
  ::at(400.0f, 160.0f, true);
  ::look("race");
  ::at(400.0f, 160.0f, false);
  ::look("release");
  ::at(120.0f, 100.0f, true);
  ::look("first");
  ::at(120.0f, 100.0f, false);
  ::look("once");
  ::at(120.0f, 100.0f, true);
  ::look("again");
  ::at(120.0f, 100.0f, false);
  ::look("restored");
  ::at(100.0f, 210.0f, true);
  ::look("throw");
  ::at(100.0f, 210.0f, false);
  ::look("thrown");
  ::at(100.0f, 210.0f, true);
  ::look("again thrown");
  ::at(100.0f, 210.0f, false);
  ::look("returned");
  ::at(32.0f, 420.0f, true);
  ::look("lands");
  ::at(216.0f, 420.0f, true);
  ::look("rides");
  ::at(216.0f, 420.0f, false);
  ::look("parts");
  status |= check("a slider never seats", GUI::GET::editing(::bench).empty());
  ::at(316.0f, 420.0f, true);
  ::look("taps");
  ::at(316.0f, 420.0f, false);
  ::look("tapped");
  ::at(316.0f, 420.0f, true);
  ::look("taps again");
  ::at(316.0f, 420.0f, false);
  ::look("held to the point");
  status |= check("focus", GUI::focus(::bench, true) == 0);
  ::at(0.0f, 0.0f, false);
  status |= check("release the seat", GUI::edit(::bench, "") == 0);
  status |= check("seat the field", GUI::edit(::bench, "name") == 0);
  typed({{INPUT::KEYS::TEXT, INPUT::KEYS::TAB}});
  ::look("ring");
  ::press(INPUT::KEYS::RIGHT);
  ::look("nudge");
  ::press(INPUT::KEYS::END);
  ::look("end");
  ::press(INPUT::KEYS::HOME);
  ::look("home");
  ::press(INPUT::KEYS::ENTER);
  ::look("open");
  typed(
    {{INPUT::KEYS::TEXT, '0'},
     {INPUT::KEYS::TEXT, '.'},
     {INPUT::KEYS::TEXT, '5'}});
  ::look("typed");
  status |= check("edited", GUI::GET::edited(::bench, "gain"));
  ::press(INPUT::KEYS::ENTER);
  ::look("commit");
  status |= check("committed", GUI::GET::committed(::bench, "gain"));
  ::press(INPUT::KEYS::RIGHT);
  ::look("nudges again");
  typed({{INPUT::KEYS::TEXT, INPUT::KEYS::TAB}});
  ::look("steps on");
  typed({{INPUT::KEYS::TEXT, INPUT::KEYS::TAB}});
  ::look("reaches the flag");
  ::press(INPUT::KEYS::ENTER);
  ::look("keyed throw");
  typed({{INPUT::KEYS::TEXT, INPUT::KEYS::TAB}});
  ::look("reaches the slider");
  ::press(INPUT::KEYS::ENTER);
  ::look("stays shut");
  ::press(INPUT::KEYS::RIGHT);
  ::look("nudged");
  ::press(INPUT::KEYS::DOWN);
  ::look("hands");
  status |=
    check("pull refuses a stranger", GUI::drag(::bench, "ghost", 0, 0, 1) != 0);
  status |= check("stage", GUI::stage(::bench) == 0);
  status |= check("pull once", GUI::drag(::bench, "free", 50.0f, 0, 1) == 0);
  ::look("once pulled");
  const Float crept = GUI::GET::value(::bench, "free");
  status |= check("stage again", GUI::stage(::bench) == 0);
  status |= check("pull thrice", GUI::drag(::bench, "free", 50.0f, 0, 3) == 0);
  ::look("thrice pulled");
  status |= check("holds count", GUI::GET::value(::bench, "free") > crept);
  ::metrics(LINE, ADVANCE);
  status |= check("release the seat", GUI::edit(::bench, "") == 0);
  ::at(0.0f, 0.0f, false);
  ::look("measured");
  ::at(56.0f, 100.0f, true);
  ::look("presses");
  ::at(56.0f, 100.0f, false);
  ::look("opens");
  ::at(40.0f, 100.0f, true);
  ::look("anchors");
  ::at(72.0f, 100.0f, true);
  ::look("marks");
  ::at(72.0f, 100.0f, false);
  ::look("holds");
  status |= check("release the seat again", GUI::edit(::bench, "") == 0);
  ::at(40.0f, 100.0f, true);
  ::look("regrabs");
  ::at(72.0f, 100.0f, true);
  ::look("scrubs");
  ::at(72.0f, 100.0f, false);
  ::look("dropped");
  return status;
}
