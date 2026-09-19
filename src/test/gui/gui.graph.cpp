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
  "  node amp x=140 y=10 w=80 h=40 text=\"amp\"\n"
  "    label amp.in x=0 y=8 w=10 h=10 port=in,MIDI\n"
  "    label amp.out x=70 y=8 w=10 h=10 port=out,audio\n"
  "  node out x=140 y=100 w=80 h=40 port=in,audio text=\"out\"\n"
  "button away x=8 y=210 w=60 h=20 text=\"away\"\n";

Whole written = 0;

auto name(GUI::Event::Kind kind) -> STRING::Hot {
  static constexpr STRING::Hot WORDS[] = {
    "entered",   "exited", "pressed",   "clicked", "selected",
    "activated", "edited", "committed", "dialled", "moved"};
  return WORDS[kind];
}

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
  std::cout << std::fixed << std::setprecision(1) << "graph " << name << ":";
  ::where("osc");
  ::where("amp");
  ::where("out");
  std::cout << " hover=" << (over.empty() ? "-" : over) << " events=[";
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

void wires() {
  std::cout << "graph links:";
  for (const auto &link : GUI::NGA::GET::links(::bench, "")) {
    std::cout << " " << link.from << ">" << link.to;
    if (!link.label.empty()) std::cout << "=" << link.label;
  }
  std::cout << std::endl;
}

}  // namespace

Status graph() {
  const String layout = LAYOUT;
  rescale(2.0f);
  ::bench = GUI::load("graph", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("graph load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  using Side = GUI::NGA::Port::Side;
  status |=
    check("port read", GUI::NGA::GET::port(::bench, "out").side == Side::IN);
  status |=
    check("no port", GUI::NGA::GET::port(::bench, "osc").side == Side::NONE);
  status |= check("no kind", GUI::NGA::GET::port(::bench, "osc").kind.empty());
  status |= check(
    "port write",
    GUI::NGA::set(::bench, "osc", GUI::NGA::Port{Side::OUT, "audio"}) == 0);
  status |=
    check("port wrote", GUI::NGA::GET::port(::bench, "osc").side == Side::OUT);
  status |=
    check("kind wrote", GUI::NGA::GET::port(::bench, "osc").kind == "audio");
  status |=
    check("kind read", GUI::NGA::GET::port(::bench, "amp.in").kind == "MIDI");
  status |= check(
    "refuse no scale",
    GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{0.0f}) != 0);
  status |=
    check("scale stands", GUI::NGA::GET::zoom(::bench, "deck").value == 1.0f);
  ::at(0.0f, 0.0f, false);
  ::look("rest");
  status |= check(
    "pan", GUI::NGA::set(::bench, "deck", GUI::NGA::Pan{20.0f, 10.0f}) == 0);
  ::look("panned");
  status |=
    check("zoom", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f}) == 0);
  ::look("zoomed");
  const auto uniform = GUI::NGA::GET::zoom(::bench, "deck");
  status |=
    check("uniform pair", uniform.value == 2.0f && uniform.down == 2.0f);
  status |= check(
    "stretch", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f, 1.0f}) == 0);
  ::look("stretched");
  const auto apart = GUI::NGA::GET::zoom(::bench, "deck");
  status |= check("axes apart", apart.value == 2.0f && apart.down == 1.0f);
  status |= check(
    "refuse no down scale",
    GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f, -1.0f}) != 0);
  const String sheet = "graph board x=0 y=0 w=100 h=100 zoom=2,1\n";
  const auto stated =
    GUI::load("stated", Vector<Byte>(sheet.begin(), sheet.end()));
  status |= check("parse a pair", stated != GUI::NONE);
  const auto authored = GUI::NGA::GET::zoom(stated, "board");
  status |=
    check("pair parsed", authored.value == 2.0f && authored.down == 1.0f);
  GUI::remove(stated);
  status |= check(
    "uniform again", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f}) == 0);
  ::look("uniform");
  ::at(120.0f, 100.0f, false);
  ::look("over");
  status |= check("hovered under zoom", GUI::GET::hover(::bench) == "osc");
  status |= check(
    "view restored", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{1.0f}) == 0);
  status |= check(
    "pan restored",
    GUI::NGA::set(::bench, "deck", GUI::NGA::Pan{0.0f, 0.0f}) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("back");
  ::at(120.0f, 100.0f, true);
  ::look("press");
  ::at(124.0f, 100.0f, true);
  ::look("wobble");
  ::at(124.0f, 100.0f, false);
  ::look("click");
  ::at(120.0f, 100.0f, true);
  ::look("grab");
  ::at(200.0f, 160.0f, true);
  ::look("carry");
  status |= check("moved answers once", GUI::GET::moved(::bench, "osc"));
  status |= check("moved consumed", !GUI::GET::moved(::bench, "osc"));
  ::at(200.0f, 160.0f, false);
  ::look("drop");
  status |=
    check("rezoom", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f}) == 0);
  ::look("rescaled");
  ::at(280.0f, 240.0f, true);
  ::look("hold");
  ::at(360.0f, 240.0f, true);
  ::look("scaled carry");
  ::at(280.0f, 200.0f, false);
  ::look("let go");
  status |=
    check("unzoom", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{1.0f}) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("settled");
  status |=
    check("drag refuses a stranger", GUI::drag(::bench, "osc", "ghost") != 0);
  status |= check("stage", GUI::stage(::bench) == 0);
  status |= check("drag", GUI::drag(::bench, "amp", "out") == 0);
  ::look("driven");
  status |= check("drag moved", GUI::GET::moved(::bench, "amp"));
  status |= check("stage again", GUI::stage(::bench) == 0);
  status |= check("click", GUI::click(::bench, "away") == 0);
  ::look("clicked");
  status |= check("connect", GUI::NGA::connect(::bench, "osc", "out") == 0);
  status |= check("linked", GUI::NGA::GET::linked(::bench, "osc", "out"));
  status |= check("directed", !GUI::NGA::GET::linked(::bench, "out", "osc"));
  status |=
    check("again is a no-op", GUI::NGA::connect(::bench, "osc", "out") == 0);
  status |= check("refuse self", GUI::NGA::connect(::bench, "osc", "osc") != 0);
  status |=
    check("refuse a stranger", GUI::NGA::connect(::bench, "osc", "ghost") != 0);
  status |= check(
    "refuse off the board", GUI::NGA::connect(::bench, "osc", "away") != 0);
  status |= check(
    "refuse a cross kind",
    GUI::NGA::connect(::bench, "amp.out", "amp.in") != 0);
  status |= check(
    "connect a socket",
    GUI::NGA::connect(::bench, "amp.out", "out", GUI::NGA::Label{"excites"}) ==
      0);
  status |= check(
    "label", GUI::NGA::GET::label(::bench, "amp.out", "out") == "excites");
  const auto sockets = GUI::NGA::GET::ports(::bench, "amp");
  status |= check("ports", sockets.size() == 2 && sockets[0] == "amp.in");
  status |=
    check("standing links", GUI::NGA::GET::links(::bench, "out").size() == 2);
  ::wires();
  ::look("wired");
  status |= check("stage to wire", GUI::stage(::bench) == 0);
  status |= check("carry a wired node", GUI::drag(::bench, "osc", "amp") == 0);
  ::look("wire follows");
  status |= check(
    "rename",
    GUI::NGA::connect(::bench, "osc", "out", GUI::NGA::Label{"drives"}) == 0);
  status |=
    check("one link still", GUI::NGA::GET::links(::bench, "").size() == 2);
  status |=
    check("disconnect", GUI::NGA::disconnect(::bench, "osc", "out") == 0);
  status |=
    check("parted once", GUI::NGA::disconnect(::bench, "osc", "out") != 0);
  ::wires();
  status |= check("remove a port", GUI::NODES::remove(::bench, "amp.out") == 0);
  ::wires();
  status |= check("retired", GUI::NGA::GET::links(::bench, "").empty());
  ::look("unwired");
  return status;
}
