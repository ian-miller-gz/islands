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

constexpr Float PIXELS = 2.0f;

constexpr STRING::Hot LAYOUT =
  "graph deck x=20 y=20 w=280 h=180\n"
  "  node osc x=10 y=10 w=60 h=30 text=\"osc\"\n"
  "    label osc.out x=48 y=10 w=10 h=10 port=out\n"
  "  node amp x=110 y=10 w=60 h=30 text=\"amp\"\n"
  "    label amp.in x=2 y=10 w=10 h=10 port=in\n"
  "  node out x=110 y=90 w=60 h=30 port=in text=\"out\"\n"
  "list roster x=8 y=210 w=120 h=60 rows=3\n"
  "graph band x=140 y=204 w=160 h=32 choosing=band\n"
  "  node lone x=10 y=2 w=80 h=28 text=\"lone\"\n"
  "    label lone.out x=64 y=8 w=12 h=12 port=out\n"
  "panel strip x=304 y=20 w=14 h=100\n";

Whole written = 0;
GUI::Position pointer;

auto name(GUI::Event::Kind kind) -> STRING::Hot {
  static constexpr STRING::Hot WORDS[] = {
    "entered", "exited",    "pressed", "clicked", "selected", "activated",
    "edited",  "committed", "dialled", "moved",   "joined",   "cut"};
  return WORDS[kind];
}

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  const auto view = GUI::NGA::GET::pan(::bench, "deck");
  std::cout << std::fixed << std::setprecision(1) << "select " << name
            << ": at=" << ::pointer.x << "," << ::pointer.y << " pan=" << view.x
            << "," << view.y << " selected=[";
  STRING::Hot comma = "";
  for (const auto &id : GUI::NGA::GET::selections(::bench, "deck")) {
    std::cout << comma << id;
    comma = ",";
  }
  std::cout << "] events=[";
  comma = "";
  for (const auto &event : GUI::GET::events(::bench)) {
    std::cout << comma << event.id << " " << ::name(event.kind);
    comma = ", ";
  }
  const auto writes = GUI::GET::writes();
  std::cout << "] rewrites=" << writes - ::written << std::endl;
  ::written = writes;
}

void dress() {
  static constexpr STRING::Hot WORN[] = {"osc", "osc.out", "amp", "out"};
  std::cout << "select dress:";
  for (const auto &id : WORN) {
    const auto ink = GUI::GET::color(::bench, id);
    std::cout << " " << id << "=" << std::setprecision(2) << ink.r << ","
              << ink.g << "," << ink.b;
  }
  std::cout << std::setprecision(1) << std::endl;
}

auto asked() -> GUI::NGA::Ask { return GUI::NGA::GET::asked(::bench); }

void ask() {
  const auto answer = ::asked();
  std::cout << "select ask: asked=" << (answer.asked ? 1 : 0)
            << " at=" << answer.x << "," << answer.y
            << " board=" << answer.board << " target=" << answer.target
            << std::endl;
}

void at(Float x, Float y, Flag down) {
  ::pointer = {x, y};
  aim(x * PIXELS, y * PIXELS);
  hold(down);
}

void press(INPUT::KEYS::Action action) {
  GUI::stage(::bench);
  GUI::key(::bench, {.action = action});
}

}  // namespace

Status select() {
  const String layout = LAYOUT;
  rescale(PIXELS);
  ::bench = GUI::load("select", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("select load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("rest");
  status |= check("nothing selected", !GUI::NGA::GET::selected(::bench, "osc"));
  status |= check("stage", GUI::stage(::bench) == 0);
  status |= check("click a node", GUI::click(::bench, "osc") == 0);
  ::look("selected");
  status |= check("selected", GUI::NGA::GET::selected(::bench, "osc"));
  status |= check("board said so", GUI::GET::selected(::bench, "deck"));
  ::dress();
  status |= check("stage a socket", GUI::stage(::bench) == 0);
  status |= check("click a socket", GUI::click(::bench, "osc.out") == 0);
  ::look("socket");
  status |= check(
    "one selection", GUI::NGA::GET::selections(::bench, "deck").size() == 1);
  ::dress();
  ::press(INPUT::KEYS::RIGHT);
  ::look("stepped");
  ::press(INPUT::KEYS::RIGHT);
  ::look("stepped again");
  status |= check("walked", GUI::NGA::GET::selected(::bench, "amp"));
  ::press(INPUT::KEYS::DOWN);
  ::look("dropped");
  status |= check("down a row", GUI::NGA::GET::selected(::bench, "out"));
  ::press(INPUT::KEYS::DOWN);
  ::look("edge");
  ::press(INPUT::KEYS::HOME);
  ::look("home");
  ::press(INPUT::KEYS::END);
  ::look("end");
  ::press(INPUT::KEYS::ENTER);
  ::look("taken");
  status |= check("activated", GUI::GET::activated(::bench, "deck"));
  ::at(25.0f, 25.0f, true);
  ::look("banded");
  ::at(120.0f, 50.0f, true);
  ::look("stretching");
  ::at(200.0f, 70.0f, true);
  ::look("over two");
  ::at(200.0f, 70.0f, false);
  ::look("caught");
  status |= check("caught two boxes", GUI::NGA::GET::selected(::bench, "amp"));
  status |= check("and the first", GUI::NGA::GET::selected(::bench, "osc"));
  ::at(190.0f, 220.0f, true);
  ::look("banded press");
  ::at(190.0f, 220.0f, false);
  ::look("banded click");
  status |= check(
    "a click chooses nothing on a banded board",
    !GUI::NGA::GET::selected(::bench, "lone"));
  ::at(250.0f, 170.0f, true);
  ::look("bare press");
  ::at(250.0f, 170.0f, false);
  ::look("cleared");
  status |=
    check("cleared", GUI::NGA::GET::selections(::bench, "deck").empty());
  ::press(INPUT::KEYS::DOWN);
  ::look("list steers");
  status |= check("the list moved", GUI::GET::cursor(::bench, "roster") == 1);
  grip(true);
  ::at(150.0f, 170.0f, false);
  ::look("grabbed");
  ::at(130.0f, 150.0f, false);
  ::look("panned");
  grip(false);
  ::at(130.0f, 150.0f, false);
  ::look("let go");
  ::ask();
  status |=
    check("the view moved", GUI::NGA::GET::pan(::bench, "deck").x == 20.0f);
  status |= check("the pan asked nothing", ::asked().board.empty());
  status |= check("stage under pan", GUI::stage(::bench) == 0);
  status |= check("click under pan", GUI::click(::bench, "amp") == 0);
  ::look("hit under pan");
  status |= check("picked", GUI::NGA::GET::selected(::bench, "amp"));
  grip(true);
  ::at(140.0f, 30.0f, false);
  ::look("asking a node");
  ::ask();
  grip(false);
  ::at(140.0f, 30.0f, false);
  ::look("asked a node");
  ::ask();
  status |= check("the node answered", ::asked().target == "amp");
  status |= check("its board said so", ::asked().board == "deck");
  status |= check(
    "the view held still", GUI::NGA::GET::pan(::bench, "deck").x == 20.0f);
  grip(true);
  ::at(250.0f, 170.0f, false);
  ::look("asking the board");
  grip(false);
  ::at(250.0f, 170.0f, false);
  ::look("asked the board");
  ::ask();
  status |= check("the bare board answered", ::asked().board == "deck");
  status |= check("and named no node", ::asked().target.empty());
  grip(true);
  ::at(60.0f, 225.0f, false);
  ::look("asking off the board");
  grip(false);
  ::at(60.0f, 225.0f, false);
  ::look("asked off the board");
  ::ask();
  status |= check("the widget answered", ::asked().target == "roster.0");
  status |= check("and no board", ::asked().board.empty());
  status |= check("an ask stood", ::asked().asked);
  grip(true);
  ::at(311.0f, 70.0f, false);
  ::look("asking a strip");
  grip(false);
  ::at(311.0f, 70.0f, false);
  ::look("asked a strip");
  ::ask();
  status |= check("the strip answered", ::asked().target == "strip");
  grip(true);
  ::at(5.0f, 5.0f, false);
  ::look("asking nothing");
  grip(false);
  ::at(5.0f, 5.0f, false);
  ::look("asked nothing");
  ::ask();
  status |= check("nothing asked", !::asked().asked);
  status |= check("stage the drive", GUI::stage(::bench) == 0);
  status |= check(
    "the drive refuses on nothing",
    GUI::ask(::bench, "deck", -158.0f, -108.0f) != 0);
  status |= check("stage it again", GUI::stage(::bench) == 0);
  status |= check(
    "the drive asks a plain widget",
    GUI::ask(::bench, "strip", 0.0f, 0.0f) == 0);
  ::look("driven off the board");
  ::ask();
  status |= check("the driven widget answered", ::asked().target == "strip");
  status |= check(
    "the band chooses by the band",
    GUI::NGA::GET::choosing(::bench, "band").band);
  status |= check("stage the banded", GUI::stage(::bench) == 0);
  status |= check("click the lone node", GUI::click(::bench, "lone") == 0);
  ::look("banded board");
  status |= check("chose nothing", !GUI::NGA::GET::selected(::bench, "lone"));
  status |= check("stage the lone socket", GUI::stage(::bench) == 0);
  status |=
    check("click the lone socket", GUI::click(::bench, "lone.out") == 0);
  ::look("banded socket");
  status |=
    check("still nothing", GUI::NGA::GET::selections(::bench, "band").empty());
  status |= check(
    "the write chooses",
    GUI::NGA::set(::bench, "lone", GUI::NGA::Selected{true}) == 0 &&
      GUI::NGA::GET::selected(::bench, "lone"));
  return status;
}
