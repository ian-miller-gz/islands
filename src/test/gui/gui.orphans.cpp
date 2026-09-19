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
  "  node seed x=190 y=10 w=60 h=30 port=out text=\"seed\"\n"
  "  node one x=10 y=10 w=60 h=30 text=\"one\"\n"
  "    label one.in x=4 y=10 w=10 h=10 port=in\n"
  "    label one.out x=46 y=10 w=10 h=10 port=out\n"
  "  node two x=100 y=10 w=60 h=30 text=\"two\"\n"
  "    label two.in x=4 y=10 w=10 h=10 port=in\n"
  "    label two.out x=46 y=10 w=10 h=10 port=out\n";

void loses(STRING::Hot id) {
  Vector<STRING::Cold> boxes;
  const Status told = GUI::NGA::GET::orphans(::bench, id, boxes);
  std::cout << "orphans " << id << ":";
  if (told != 0) {
    std::cout << " refused, " << boxes.size() << " named" << std::endl;
    return;
  }
  if (boxes.empty()) std::cout << " none";
  for (const auto &box : boxes) std::cout << " " << box;
  std::cout << std::endl;
}

void join(STRING::Hot from, STRING::Hot to) {
  const Status told = GUI::NGA::connect(::bench, from, to);
  std::cout << "join " << from << ">" << to << ": "
            << (told == 0 ? "made" : "refused") << std::endl;
}

}  // namespace

Status orphans() {
  const String layout = LAYOUT;
  ::bench = GUI::load("orphans", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("orphans load", ::bench != GUI::NONE);
  ::loses("osc");
  ::join("osc.out", "amp.in");
  ::join("amp.out", "out");
  ::loses("amp");
  ::loses("out");
  ::loses("osc");
  ::loses("lfo");
  ::loses("osc.out");
  Vector<STRING::Cold> boxes;
  status |= check(
    "the chain's head takes the rest",
    GUI::NGA::GET::orphans(::bench, "osc", boxes) == 0 && boxes.size() == 2);
  ::join("feed.out", "dry.in");
  ::join("feed.out", "wet.in");
  ::join("dry.out", "sum");
  ::join("wet.out", "sum");
  ::loses("dry");
  ::loses("wet");
  ::loses("feed");
  status |= check(
    "a fed sink survives one arm",
    GUI::NGA::GET::orphans(::bench, "dry", boxes) == 0 && boxes.empty());
  GUI::NGA::disconnect(::bench, "wet.out", "sum");
  ::loses("dry");
  ::join("top", "gate");
  ::join("gate", "tail");
  ::loses("top");
  ::loses("gate");
  ::join("one.out", "two.in");
  ::join("two.out", "one.in");
  ::loses("one");
  ::loses("two");
  ::join("seed", "one.in");
  ::loses("seed");
  ::loses("one");
  ::loses("ghost");
  status |= check(
    "an unknown document refuses",
    GUI::NGA::GET::orphans(GUI::NONE, "osc", boxes) != 0);
  status |= check("and names nobody", boxes.empty());
  return status;
}
