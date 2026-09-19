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
  "graph deck x=20 y=20 w=280 h=220\n"
  "  node a x=10 y=80 w=60 h=60 text=\"a\"\n"
  "    label a.0 x=48 y=4 w=12 h=12 port=out\n"
  "    label a.1 x=48 y=24 w=12 h=12 port=out\n"
  "    label a.2 x=48 y=44 w=12 h=12 port=out\n"
  "  node b x=180 y=150 w=60 h=60 text=\"b\"\n"
  "    label b.0 x=0 y=4 w=12 h=12 port=in\n"
  "    label b.1 x=0 y=24 w=12 h=12 port=in\n"
  "    label b.2 x=0 y=44 w=12 h=12 port=in\n"
  "  node c x=180 y=10 w=60 h=60 text=\"c\"\n"
  "    label c.0 x=0 y=4 w=12 h=12 port=in\n"
  "    label c.1 x=0 y=24 w=12 h=12 port=in\n"
  "    label c.2 x=0 y=44 w=12 h=12 port=in\n";

constexpr STRING::Hot AROUND =
  "graph deck x=10 y=10 w=300 h=220\n"
  "  node d x=50 y=10 w=50 h=40 text=\"d\"\n"
  "    label d.0 x=38 y=4 w=12 h=12 port=out\n"
  "    label d.1 x=38 y=20 w=12 h=12 port=out\n"
  "  node e x=50 y=90 w=50 h=40 text=\"e\"\n"
  "    label e.0 x=0 y=4 w=12 h=12 port=in\n"
  "    label e.1 x=0 y=20 w=12 h=12 port=in\n"
  "  node f x=180 y=150 w=50 h=40 text=\"f\"\n"
  "    label f.0 x=38 y=4 w=12 h=12 port=out\n"
  "  node g x=50 y=150 w=50 h=40 text=\"g\"\n"
  "    label g.0 x=0 y=4 w=12 h=12 port=in\n";

GUI::Position pointer;

auto name(GUI::Event::Kind kind) -> STRING::Hot {
  static constexpr STRING::Hot WORDS[] = {
    "entered", "exited",    "pressed", "clicked", "selected", "activated",
    "edited",  "committed", "dialled", "moved",   "joined",   "cut"};
  return WORDS[kind];
}

void band(STRING::Hot name, const Vector<GUI::NGA::Link> &links) {
  std::cout << " " << name << "=[";
  STRING::Hot comma = "";
  for (const auto &link : links) {
    std::cout << comma << " " << link.from << ">" << link.to;
    comma = ",";
  }
  std::cout << " ]";
}

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  std::cout << std::fixed << std::setprecision(1) << "route " << name
            << ": at=" << ::pointer.x << "," << ::pointer.y;
  ::band("links", GUI::NGA::GET::links(::bench, ""));
  ::band("cut", GUI::NGA::GET::cut(::bench));
  std::cout << " events=[";
  STRING::Hot comma = "";
  for (const auto &event : GUI::GET::events(::bench)) {
    std::cout << comma << event.id << " " << ::name(event.kind);
    comma = ", ";
  }
  std::cout << "]" << std::endl;
}

void at(Float x, Float y, Flag down) {
  ::pointer = {x, y};
  aim(x * PIXELS, y * PIXELS);
  hold(down);
}

constexpr Float PULL = 5.0f;
auto stroke(STRING::Hot name, Float x, Float y) -> Vector<GUI::NGA::Link> {
  ::at(x, y, true);
  ::look(name);
  ::at(x, y + PULL, true);
  ::look("pulled");
  const auto cuts = GUI::NGA::GET::cut(::bench);
  ::at(x, y + PULL, false);
  ::look("released");
  return cuts;
}

auto parted(
  const Vector<GUI::NGA::Link> &cuts, STRING::Hot from,
  STRING::Hot to) -> Flag {
  return cuts.size() == 1 && cuts[0].from == from && cuts[0].to == to;
}

Status bundle(STRING::Hot sink) {
  Status status = 0;
  for (STRING::Hot row : {"0", "1", "2"}) {
    const String from = String("a.") + row;
    const String to = String(sink) + "." + row;
    status |=
      check("join", GUI::NGA::connect(::bench, from.c_str(), to.c_str()) == 0);
  }
  return status;
}

Status piled() {
  Status status =
    check("stacked", GUI::NGA::connect(::bench, "d.0", "e.0") == 0);
  status |=
    check("stacked again", GUI::NGA::connect(::bench, "d.1", "e.1") == 0);
  ::look("stacked");
  status |= check(
    "nothing at the halfway column",
    ::stroke("halfway, stacked", 85.0f, 70.0f).empty());
  status |= check(
    "nothing between the socket and its stub",
    ::stroke("short of the stub", 108.0f, 70.0f).empty());
  status |= check(
    "nor between the sink's stub and its socket",
    ::stroke("past the stub", 62.0f, 70.0f).empty());
  status |= check(
    "out of the source by STUB, then down",
    ::parted(::stroke("the source's stub", 116.0f, 70.0f), "d.0", "e.0"));
  status |= check("rejoined", GUI::NGA::connect(::bench, "d.0", "e.0") == 0);
  status |= check(
    "the channel midway between the two boxes",
    ::parted(::stroke("the channel", 85.0f, 80.0f), "d.0", "e.0"));
  status |=
    check("rejoined again", GUI::NGA::connect(::bench, "d.0", "e.0") == 0);
  status |= check(
    "then down a column STUB west of the sink",
    ::parted(::stroke("the sink's stub", 54.0f, 96.0f), "d.0", "e.0"));
  status |= check(
    "the fan steps the channel south",
    ::parted(::stroke("the fanned channel", 85.0f, 92.0f), "d.1", "e.1"));
  return status;
}

Status levelled() {
  Status status = check("level", GUI::NGA::connect(::bench, "f.0", "g.0") == 0);
  ::look("level");
  status |= check(
    "nothing on the sockets' own row",
    ::stroke("between the boxes", 150.0f, 170.0f).empty());
  status |= check(
    "and nothing above the lower foot",
    ::stroke("short of the foot", 150.0f, 199.0f).empty());
  status |= check(
    "the channel STUB below the lower foot",
    ::parted(::stroke("under both", 150.0f, 212.0f), "f.0", "g.0"));
  status |= check("relevelled", GUI::NGA::connect(::bench, "f.0", "g.0") == 0);
  status |= check(
    "east out of the source, then down past its foot",
    ::parted(::stroke("the source's stub", 246.0f, 190.0f), "f.0", "g.0"));
  status |=
    check("relevelled again", GUI::NGA::connect(::bench, "f.0", "g.0") == 0);
  status |= check(
    "and back up a column STUB west of the sink",
    ::parted(::stroke("the sink's stub", 54.0f, 190.0f), "f.0", "g.0"));
  return status;
}

}  // namespace

Status routed() {
  const String layout = LAYOUT;
  rescale(PIXELS);
  ::bench = GUI::load("route", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("route load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("rest");
  status |= ::bundle("b");
  ::look("descending");
  status |= check(
    "the first pair at the halfway column",
    ::parted(::stroke("halfway", 145.0f, 165.0f), "a.0", "b.0"));
  status |=
    check("nothing between", ::stroke("between", 139.0f, 165.0f).empty());
  status |= check(
    "the second pair two steps west",
    ::parted(::stroke("one pair west", 133.0f, 165.0f), "a.1", "b.1"));
  status |= check(
    "the third pair four steps west",
    ::parted(::stroke("two pairs west", 121.0f, 165.0f), "a.2", "b.2"));
  status |= check("board cleared", GUI::NGA::GET::links(::bench, "").empty());
  status |= ::bundle("c");
  ::look("climbing");
  status |= check(
    "nothing west while climbing",
    ::stroke("west of halfway", 133.0f, 95.0f).empty());
  status |= check(
    "the second pair two steps east",
    ::parted(::stroke("one pair east", 157.0f, 95.0f), "a.1", "c.1"));
  status |= check(
    "the first pair still halfway",
    ::parted(::stroke("halfway again", 145.0f, 95.0f), "a.0", "c.0"));
  status |= check("the third parted", GUI::NGA::clear(::bench, "a.2") == 0);
  status |= check("row to row", GUI::NGA::connect(::bench, "a.0", "b.0") == 0);
  status |= check("down a row", GUI::NGA::connect(::bench, "a.0", "b.1") == 0);
  status |= check("up a row", GUI::NGA::connect(::bench, "a.1", "b.0") == 0);
  ::look("fanned");
  status |= check(
    "the sink's rank stepped it",
    ::parted(::stroke("one step, low", 139.0f, 190.0f), "a.0", "b.1"));
  status |= check(
    "the source's rank stepped it",
    ::parted(::stroke("one step, high", 139.0f, 150.0f), "a.1", "b.0"));
  status |= check(
    "the row-to-row wire kept halfway",
    ::parted(::stroke("halfway, last", 145.0f, 165.0f), "a.0", "b.0"));
  const String around = AROUND;
  ::bench = GUI::load("around", Vector<Byte>(around.begin(), around.end()));
  status |= check("around load", ::bench != GUI::NONE);
  status |= check("around place", GUI::place(::bench, WHOLE) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("around");
  status |= ::piled();
  status |= ::levelled();
  status |= check("around cleared", GUI::NGA::GET::links(::bench, "").empty());
  return status;
}
