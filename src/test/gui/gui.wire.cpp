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
  "style dressed edge=#ff0000ff\n"
  "graph deck x=20 y=20 w=280 h=180\n"
  "  node osc x=10 y=10 w=80 h=40 text=\"osc\"\n"
  "    label osc.out x=64 y=14 w=12 h=12 port=out\n"
  "  node amp x=140 y=10 w=80 h=40 text=\"amp\"\n"
  "    label amp.in x=4 y=14 w=12 h=12 port=in\n"
  "    label amp.out x=64 y=14 w=12 h=12 port=out\n"
  "  node out x=140 y=100 w=80 h=40 port=in text=\"out\"\n";

Whole written = 0;
Whole cost = 0;
GUI::Position pointer;

auto name(GUI::Event::Kind kind) -> STRING::Hot {
  static constexpr STRING::Hot WORDS[] = {
    "entered", "exited",    "pressed", "clicked", "selected", "activated",
    "edited",  "committed", "dialled", "moved",   "joined",   "cut"};
  return WORDS[kind];
}

void pair(const GUI::NGA::Link &link) {
  std::cout << " " << link.from << ">" << link.to;
  if (!link.style.empty()) std::cout << ":" << link.style;
}

void band(STRING::Hot name, const Vector<GUI::NGA::Link> &links) {
  std::cout << " " << name << "=[";
  STRING::Hot comma = "";
  for (const auto &link : links) {
    std::cout << comma;
    comma = ",";
    ::pair(link);
  }
  std::cout << " ]";
}

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::bench, pass);
  const auto held = GUI::NGA::GET::wiring(::bench);
  const auto over = GUI::GET::hover(::bench);
  std::cout << std::fixed << std::setprecision(1) << "wire " << name
            << ": at=" << ::pointer.x << "," << ::pointer.y
            << " wiring=" << (held.empty() ? "-" : held)
            << " hover=" << (over.empty() ? "-" : over);
  ::band("links", GUI::NGA::GET::links(::bench, ""));
  ::band("joined", GUI::NGA::GET::joined(::bench));
  ::band("cut", GUI::NGA::GET::cut(::bench));
  std::cout << " events=[";
  STRING::Hot comma = "";
  for (const auto &event : GUI::GET::events(::bench)) {
    std::cout << comma << event.id << " " << ::name(event.kind);
    comma = ", ";
  }
  const auto writes = GUI::GET::writes();
  ::cost = writes - ::written;
  std::cout << "] rewrites=" << ::cost << std::endl;
  ::written = writes;
}

void at(Float x, Float y, Flag down) {
  ::pointer = {x, y};
  aim(x * PIXELS, y * PIXELS);
  hold(down);
}

}  // namespace

Status wire() {
  const String layout = LAYOUT;
  rescale(PIXELS);
  ::bench = GUI::load("wire", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("wire load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("rest");
  ::at(100.0f, 50.0f, false);
  ::look("over");
  status |= check("a port is picked", GUI::GET::hover(::bench) == "osc.out");
  ::at(100.0f, 50.0f, true);
  ::look("hitched");
  status |=
    check("the wire is pending", GUI::NGA::GET::wiring(::bench) == "osc.out");
  ::at(160.0f, 110.0f, true);
  ::look("reaching");
  ::at(160.0f, 110.0f, false);
  ::look("assisted");
  status |= check(
    "the assist joined", GUI::NGA::GET::linked(::bench, "osc.out", "out"));
  status |= check("gesture over", GUI::NGA::GET::wiring(::bench).empty());
  ::at(100.0f, 50.0f, true);
  ::look("pressed");
  ::at(170.0f, 50.0f, true);
  ::look("over a port");
  ::at(170.0f, 50.0f, false);
  ::look("joined");
  status |=
    check("linked", GUI::NGA::GET::linked(::bench, "osc.out", "amp.in"));
  status |= check("joins answer", GUI::NGA::GET::joined(::bench).size() == 1);
  ::at(200.0f, 140.0f, true);
  ::look("on a plate");
  status |= check("a plate carries", GUI::NGA::GET::wiring(::bench).empty());
  ::at(200.0f, 140.0f, false);
  ::look("let go");
  status |= check("stage", GUI::stage(::bench) == 0);
  status |= check("drive", GUI::drag(::bench, "amp.out", "out") == 0);
  ::look("driven");
  status |=
    check("driven join", GUI::NGA::GET::linked(::bench, "amp.out", "out"));
  ::at(188.0f, 110.0f, true);
  ::look("stroked");
  ::at(215.0f, 110.0f, true);
  ::look("across");
  ::at(215.0f, 110.0f, false);
  ::look("dropped");
  status |=
    check("cut parted", !GUI::NGA::GET::linked(::bench, "amp.out", "out"));
  ::at(135.0f, 50.0f, true);
  ::look("clicked");
  ::at(135.0f, 50.0f, false);
  ::look("bare");
  status |= check(
    "a click parts nothing", GUI::NGA::GET::links(::bench, "").size() == 2);
  ::at(135.0f, 50.0f, true);
  ::look("grabbed");
  ::at(135.0f, 60.0f, true);
  ::look("pulled through");
  ::at(135.0f, 60.0f, false);
  ::look("let go of");
  status |= check("board cleared", GUI::NGA::GET::links(::bench, "").empty());
  using Side = GUI::NGA::Port::Side;
  status |= check(
    "the source is audio",
    GUI::NGA::set(::bench, "osc.out", GUI::NGA::Port{Side::OUT, "audio"}) == 0);
  status |= check(
    "the sink is MIDI",
    GUI::NGA::set(::bench, "amp.in", GUI::NGA::Port{Side::IN, "MIDI"}) == 0);
  status |= check("stage the drive", GUI::stage(::bench) == 0);
  status |= check("drive", GUI::drag(::bench, "osc.out", "amp.in") == 0);
  ::look("crossed");
  status |= check(
    "cross-kind refused", !GUI::NGA::GET::linked(::bench, "osc.out", "amp.in"));
  status |= check(
    "the sink is audio",
    GUI::NGA::set(::bench, "amp.in", GUI::NGA::Port{Side::IN, "audio"}) == 0);
  status |= check("stage again", GUI::stage(::bench) == 0);
  status |= check("drive again", GUI::drag(::bench, "osc.out", "amp.in") == 0);
  ::look("agreed");
  status |=
    check("kinds agree", GUI::NGA::GET::linked(::bench, "osc.out", "amp.in"));
  ::at(100.0f, 50.0f, true);
  ::look("aiming");
  ::at(200.0f, 100.0f, true);
  ::look("in reach");
  ::at(200.0f, 100.0f, false);
  ::look("nearest");
  const auto reached = GUI::NGA::GET::joined(::bench);
  status |= check(
    "the nearest agreeing port took it", reached.size() == 1 &&
                                           reached[0].from == "osc.out" &&
                                           reached[0].to == "out");
  status |= check(
    "the sink is MIDI",
    GUI::NGA::set(::bench, "amp.in", GUI::NGA::Port{Side::IN, "MIDI"}) == 0);
  status |= check(
    "the second source is MIDI",
    GUI::NGA::set(::bench, "amp.out", GUI::NGA::Port{Side::OUT, "MIDI"}) == 0);
  status |= check(
    "the far sink is MIDI",
    GUI::NGA::set(::bench, "out", GUI::NGA::Port{Side::IN, "MIDI"}) == 0);
  ::at(100.0f, 50.0f, true);
  ::look("aiming again");
  ::at(130.0f, 90.0f, true);
  ::look("nothing agrees");
  const auto aimless = ::cost;
  ::at(130.0f, 90.0f, false);
  ::look("refused");
  status |= check("no join at all", GUI::NGA::GET::joined(::bench).empty());
  status |= check("the wire is gone", GUI::NGA::GET::wiring(::bench).empty());
  status |= check(
    "the second source agrees",
    GUI::NGA::set(::bench, "amp.out", GUI::NGA::Port{Side::OUT, "audio"}) == 0);
  status |= check(
    "the far sink agrees",
    GUI::NGA::set(::bench, "out", GUI::NGA::Port{Side::IN, "audio"}) == 0);
  ::at(230.0f, 50.0f, true);
  ::look("aimed");
  ::at(150.0f, 110.0f, true);
  ::look("a candidate stands");
  status |= check("the guide is seated with the wire", ::cost == aimless);
  ::at(150.0f, 110.0f, false);
  ::look("guided");
  status |= check(
    "the guided release joined",
    GUI::NGA::GET::linked(::bench, "amp.out", "out"));
  status |= check(
    "the old join is parted",
    GUI::NGA::disconnect(::bench, "osc.out", "out") == 0);
  ::at(112.0f, 56.0f, false);
  ::look("near a pin");
  status |= check("the rest aims", GUI::NGA::GET::aimed(::bench) == "osc.out");
  ::at(112.0f, 56.0f, true);
  ::look("grasped");
  status |=
    check("the grasp wired", GUI::NGA::GET::wiring(::bench) == "osc.out");
  ::at(170.0f, 110.0f, true);
  ::look("carried");
  status |= check("the aim follows", GUI::NGA::GET::aimed(::bench) == "out");
  ::at(170.0f, 110.0f, false);
  ::look("landed");
  status |= check(
    "the grasped wire joined",
    GUI::NGA::GET::linked(::bench, "osc.out", "out"));
  ::at(250.0f, 170.0f, false);
  ::look("far from every pin");
  status |= check("nothing aimed", GUI::NGA::GET::aimed(::bench).empty());
  ::at(250.0f, 170.0f, true);
  ::look("banded");
  status |= check("no wire began", GUI::NGA::GET::wiring(::bench).empty());
  ::at(250.0f, 170.0f, false);
  ::look("let be");
  ::at(100.0f, 50.0f, true);
  ::look("on the pin");
  ::at(100.0f, 50.0f, false);
  ::look("clicked the pin");
  status |= check("the pin's click", GUI::GET::clicked(::bench, "osc.out"));
  status |=
    check("no join from a click", GUI::NGA::GET::joined(::bench).empty());
  ::at(112.0f, 56.0f, true);
  ::look("grasped again");
  ::at(112.0f, 56.0f, false);
  ::look("clicked near the pin");
  status |= check(
    "the grasped click is the pin's", GUI::GET::clicked(::bench, "osc.out"));
  status |= check(
    "a standing pair redressed",
    GUI::NGA::connect(::bench, "osc.out", "out", GUI::Style{"dressed"}) == 0);
  ::look("dressed");
  Flag wears = false;
  for (const auto &link : GUI::NGA::GET::links(::bench, "osc.out"))
    if (link.to == "out") wears = link.style == "dressed";
  status |= check("the link wears the style", wears);
  status |= check(
    "a fresh pair dressed at the join",
    GUI::NGA::disconnect(::bench, "amp.out", "out") == 0 &&
      GUI::NGA::connect(::bench, "amp.out", "out", GUI::Style{"dressed"}) == 0);
  ::look("joined dressed");
  status |= check(
    "undressed",
    GUI::NGA::connect(::bench, "osc.out", "out", GUI::Style{}) == 0);
  ::look("undressed");
  for (const auto &link : GUI::NGA::GET::links(::bench, "osc.out"))
    if (link.to == "out") wears = !link.style.empty();
  status |= check("the law is back", !wears);
  using Side = GUI::NGA::Port::Side;
  status |= check(
    "osc.out is muted",
    GUI::NGA::set(
      ::bench, "osc.out", GUI::NGA::Port{Side::OUT, "audio", true}) == 0);
  status |=
    check("the mute reads back", GUI::NGA::GET::port(::bench, "osc.out").mute);
  ::at(112.0f, 56.0f, false);
  ::look("near a muted pin");
  status |= check("nothing aims at it", GUI::NGA::GET::aimed(::bench).empty());
  ::at(100.0f, 50.0f, true);
  ::look("pressed a muted pin");
  status |= check("no wire began", GUI::NGA::GET::wiring(::bench).empty());
  ::at(100.0f, 50.0f, false);
  ::look("let the muted pin go");
  status |= check("stage the drag", GUI::stage(::bench) == 0);
  status |= check(
    "the drag is refused a source",
    GUI::NGA::disconnect(::bench, "osc.out", "out") == 0 &&
      GUI::drag(::bench, "osc.out", "out") == 0);
  ::look("dragged from a muted pin");
  status |= check(
    "no join from a muted source",
    !GUI::NGA::GET::linked(::bench, "osc.out", "out"));
  status |= check(
    "connect still joins it",
    GUI::NGA::connect(::bench, "osc.out", "out") == 0);
  status |= check(
    "osc.out is open again",
    GUI::NGA::set(::bench, "osc.out", GUI::NGA::Port{Side::OUT, "audio"}) == 0);
  ::at(100.0f, 50.0f, true);
  ::look("pressed the open pin");
  status |=
    check("the wire began", GUI::NGA::GET::wiring(::bench) == "osc.out");
  ::at(100.0f, 50.0f, false);
  ::look("let the open pin go");
  return status;
}
