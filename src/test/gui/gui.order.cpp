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

GUI::Handle bench = GUI::NONE;

constexpr STRING::Hot LAYOUT =
  "graph deck x=20 y=20 w=280 h=180\n"
  "  node out x=200 y=10 w=60 h=30 port=in text=\"out\"\n"
  "  node osc x=10 y=10 w=60 h=30 text=\"osc\"\n"
  "    label osc.out x=46 y=10 w=10 h=10 port=out\n"
  "  node amp x=100 y=10 w=60 h=30 text=\"amp\"\n"
  "    label amp.in x=4 y=10 w=10 h=10 port=in\n"
  "    label amp.out x=46 y=10 w=10 h=10 port=out\n"
  "  node lfo x=10 y=60 w=60 h=30 text=\"lfo\"\n"
  "    label lfo.out x=46 y=10 w=10 h=10 port=out\n"
  "graph bus x=320 y=20 w=280 h=200\n"
  "  node sum x=55 y=130 w=60 h=30 port=in text=\"sum\"\n"
  "  node wet x=100 y=70 w=60 h=30 text=\"wet\"\n"
  "    label wet.in x=4 y=10 w=10 h=10 port=in\n"
  "    label wet.out x=46 y=10 w=10 h=10 port=out\n"
  "  node dry x=10 y=70 w=60 h=30 text=\"dry\"\n"
  "    label dry.in x=4 y=10 w=10 h=10 port=in\n"
  "    label dry.out x=46 y=10 w=10 h=10 port=out\n"
  "  node feed x=10 y=10 w=60 h=30 text=\"feed\"\n"
  "    label feed.out x=46 y=10 w=10 h=10 port=out\n"
  "graph wall x=20 y=240 w=280 h=120\n"
  "  node tail x=180 y=10 w=60 h=30 port=in text=\"tail\"\n"
  "  label gate x=100 y=15 w=20 h=20 port=in\n"
  "  node top x=10 y=10 w=60 h=30 port=out text=\"top\"\n"
  "graph ring x=320 y=240 w=280 h=120\n"
  "  node one x=10 y=10 w=60 h=30 text=\"one\"\n"
  "    label one.in x=4 y=10 w=10 h=10 port=in\n"
  "    label one.out x=46 y=10 w=10 h=10 port=out\n"
  "  node two x=100 y=10 w=60 h=30 text=\"two\"\n"
  "    label two.in x=4 y=10 w=10 h=10 port=in\n"
  "    label two.out x=46 y=10 w=10 h=10 port=out\n";

void walk(STRING::Hot name, STRING::Hot id) {
  Vector<STRING::Cold> boxes;
  const Status told = GUI::NGA::GET::order(::bench, id, boxes);
  std::cout << "order " << name << ":";
  if (told != 0) {
    std::cout << " refused, " << boxes.size() << " named" << std::endl;
    return;
  }
  for (const auto &box : boxes) std::cout << " " << box;
  std::cout << std::endl;
}

void leads(STRING::Hot from, STRING::Hot to) {
  std::cout << "reaches " << from << ">" << to << ": "
            << (GUI::NGA::GET::reaches(::bench, from, to) ? "yes" : "no")
            << std::endl;
}

void join(STRING::Hot from, STRING::Hot to) {
  const Status told = GUI::NGA::connect(::bench, from, to);
  std::cout << "join " << from << ">" << to << ": "
            << (told == 0 ? "made" : "refused") << std::endl;
}

void part(STRING::Hot from, STRING::Hot to) {
  const Status told = GUI::NGA::disconnect(::bench, from, to);
  std::cout << "part " << from << ">" << to << ": "
            << (told == 0 ? "parted" : "refused") << std::endl;
}

}  // namespace

Status order() {
  const String layout = LAYOUT;
  ::bench = GUI::load("order", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("order load", ::bench != GUI::NONE);
  ::walk("deck bare", "deck");
  ::leads("osc", "out");
  ::join("osc.out", "amp.in");
  ::join("amp.out", "out");
  ::walk("deck", "deck");
  ::leads("osc", "out");
  ::leads("out", "osc");
  ::leads("osc.out", "amp.in");
  ::leads("osc", "amp");
  ::leads("osc", "lfo");
  ::leads("ghost", "out");
  status |=
    check("the chain leads", GUI::NGA::GET::reaches(::bench, "osc", "out"));
  status |=
    check("and not backwards", !GUI::NGA::GET::reaches(::bench, "out", "osc"));
  ::join("feed.out", "dry.in");
  ::join("feed.out", "wet.in");
  ::join("dry.out", "sum");
  ::join("wet.out", "sum");
  ::walk("bus", "bus");
  ::leads("feed", "sum");
  ::leads("dry", "wet");
  ::join("top", "gate");
  ::join("gate", "tail");
  ::walk("wall", "wall");
  ::leads("top", "tail");
  status |= check(
    "the terminal carries", GUI::NGA::GET::reaches(::bench, "top", "tail"));
  ::walk("page", "");
  ::leads("osc", "feed");
  ::join("one.out", "two.in");
  ::join("two.out", "one.in");
  ::walk("ring", "ring");
  ::walk("page", "");
  ::walk("deck", "deck");
  ::leads("one", "one");
  ::leads("one.out", "two.out");
  Vector<STRING::Cold> boxes;
  status |= check(
    "a ring has no order", GUI::NGA::GET::order(::bench, "ring", boxes) != 0);
  status |= check("and names nobody", boxes.empty());
  ::part("two.out", "one.in");
  ::walk("ring", "ring");
  ::leads("one", "one");
  ::join("amp.out", "amp.in");
  ::walk("deck", "deck");
  ::leads("amp", "amp");
  ::part("amp.out", "amp.in");
  ::walk("deck", "deck");
  ::walk("ghost", "ghost");
  status |= check(
    "an unknown document refuses",
    GUI::NGA::GET::order(GUI::NONE, "", boxes) != 0);
  status |= check(
    "and reaches nothing", !GUI::NGA::GET::reaches(GUI::NONE, "osc", "out"));
  return status;
}
