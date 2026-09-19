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

#include <island/gui/carry.hpp>
#include <test/gui/gui.internal.hpp>

namespace CARRY = GUI::SAC::CARRY;

namespace {

GUI::Handle deck = GUI::NONE;

constexpr STRING::Hot LAYOUT =
  "graph deck x=20 y=20 w=300 h=200\n"
  "  node one x=20 y=20 w=60 h=30 text=\"one\"\n"
  "  node two x=120 y=20 w=60 h=30 text=\"two\"\n"
  "  node three x=20 y=100 w=60 h=30 text=\"three\"\n";

constexpr Float HELD = 2.0f;
constexpr Float SEATED = 0.0f;

constexpr Float ACROSS = 24.0f;
constexpr Float DOWN = 16.0f;
constexpr Float LAST = 6.0f;

void where(STRING::Hot id) {
  const auto held = GUI::GET::position(::deck, id);
  const auto origin = GUI::GET::origin(::deck, id);
  std::cout << " " << id << "@" << held.x << "," << held.y << " +" << origin.x
            << "," << origin.y << " z" << GUI::GET::depth(::deck, id);
}

void look(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::deck, pass);
  std::cout << std::fixed << std::setprecision(1) << "carry " << name << ":";
  ::where("one");
  ::where("two");
  ::where("three");
  const auto hand = CARRY::GET::carried(::deck);
  std::cout << " hand=" << (hand.empty() ? "-" : hand) << " dropped=[";
  STRING::Hot comma = "";
  for (const auto &drop : CARRY::GET::dropped(::deck)) {
    std::cout << comma << drop.id << "@" << drop.at.x << "," << drop.at.y;
    comma = ", ";
  }
  std::cout << "]" << std::endl;
}

void selected(STRING::Hot name) {
  std::cout << "carry " << name << ": selected=[";
  STRING::Hot comma = "";
  for (const auto &id : GUI::NGA::GET::selections(::deck, "deck")) {
    std::cout << comma << id;
    comma = ", ";
  }
  std::cout << "]" << std::endl;
}

void at(Float x, Float y, Flag down) {
  aim(x, y);
  hold(down);
}

auto arranged() -> CARRY::Watch {
  return {
    "deck", {0.0f, GUI::Walls::NONE, 0.0f, GUI::Walls::NONE}, HELD, SEATED};
}

}  // namespace

Status towed() {
  const String layout = LAYOUT;
  rescale(1.0f);
  ::deck = GUI::load("carry", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("board load", ::deck != GUI::NONE);
  status |=
    check("undesignated refuses", CARRY::watch(::deck, ::arranged()) != 0);
  status |= check("adopt", GUI::SAC::adopt({String(CARRY::NAME)}) == 0);
  status |= check("watch", CARRY::watch(::deck, ::arranged()) == 0);
  status |= check("place", GUI::place(::deck, WHOLE) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("rest");
  ::at(50.0f, 55.0f, true);
  ::look("press");
  status |= check("no hold yet", CARRY::GET::carried(::deck).empty());
  ::at(90.0f, 75.0f, true);
  ::look("lone");
  status |= check("in hand", CARRY::GET::carried(::deck) == "one");
  status |= check("rides", CARRY::GET::dragged(::deck, "one"));
  status |= check("alone", !CARRY::GET::dragged(::deck, "two"));
  ::at(90.0f, 75.0f, false);
  ::look("dropped");
  status |= check("dropped one", CARRY::GET::dropped(::deck).size() == 1);
  for (STRING::Hot id : {"one", "two", "three"})
    status |=
      check("select", GUI::NGA::set(::deck, id, GUI::NGA::Selected{true}) == 0);
  ::at(150.0f, 55.0f, true);
  ::look("grab");
  ::at(110.0f, 55.0f, true);
  ::look("shape");
  status |= check("mate rides", CARRY::GET::dragged(::deck, "three"));
  status |= check("pooled", GUI::GET::position(::deck, "three").x == 0.0f);
  ::at(170.0f, 95.0f, true);
  ::look("roomy");
  status |= check("unpooled", GUI::GET::position(::deck, "three").x == 40.0f);
  ::at(170.0f, 95.0f, false);
  ::look("drop");
  const auto set = CARRY::GET::dropped(::deck);
  status |= check("whole set", set.size() == 3);
  status |= check("leader last", !set.empty() && set.back().id == "two");
  status |= check("hand empty", CARRY::GET::carried(::deck).empty());
  ::look("after");
  status |= check("one frame", CARRY::GET::dropped(::deck).empty());
  ::selected("held");
  status |= check("follow", CARRY::follow(::deck, {"three"}) == 0);
  ::selected("followed");
  status |= check(
    "a stranger is counted", CARRY::follow(::deck, {"three", "ghost"}) == 1);
  status |= check("driven press", GUI::press(::deck, "one", 0.0f, 0.0f) == 0);
  ::look("aimed");
  status |= check("driven hold waits", CARRY::GET::carried(::deck).empty());
  status |= check("first leg", GUI::travel(::deck, ACROSS, 0.0f) == 0);
  ::look("across");
  status |= check("driven in hand", CARRY::GET::carried(::deck) == "one");
  status |= check("second leg", GUI::travel(::deck, 0.0f, DOWN) == 0);
  ::look("beneath");
  status |= check("driven still in hand", CARRY::GET::carried(::deck) == "one");
  status |= check("driven lift", GUI::release(::deck, LAST, LAST) == 0);
  status |= check("moved at the lift", GUI::GET::moved(::deck, "one"));
  ::look("landed");
  status |= check("driven drop", CARRY::GET::dropped(::deck).size() == 1);
  status |= check(
    "a travel with no standing press refuses",
    GUI::travel(::deck, ACROSS, 0.0f) != 0);
  status |= check("stand down", CARRY::watch(::deck, {}) == 0);
  ::at(150.0f, 55.0f, true);
  ::at(110.0f, 55.0f, true);
  ::look("down");
  status |= check("nothing held", CARRY::GET::carried(::deck).empty());
  ::at(110.0f, 55.0f, false);
  status |= check("re-adopt", GUI::SAC::adopt({}) == 0);
  status |=
    check("the door closes again", CARRY::watch(::deck, ::arranged()) != 0);
  return status | ridden();
}
