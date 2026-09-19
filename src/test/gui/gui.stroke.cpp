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

#include <island/gui/stroke.hpp>
#include <test/gui/gui.internal.hpp>

namespace STROKE = GUI::SAC::STROKE;

namespace {

GUI::Handle deck = GUI::NONE;

constexpr STRING::Hot LAYOUT =
  "graph deck x=20 y=20 w=300 h=200\n"
  "  node box x=20 y=20 w=60 h=30 text=\"box\"\n";

constexpr Float SIDE = 20.0f;

auto pencil(STROKE::Mode::Over over = STROKE::Mode::GROUND) -> STROKE::Mode {
  return {"deck", "pencil", {SIDE, SIDE}, over};
}

void where(const STROKE::Mark &mark) {
  std::cout << mark.at.x << "," << mark.at.y << "[" << mark.cell.across << ","
            << mark.cell.down << "]";
}

void look(STRING::Hot label) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(::deck, pass);
  const auto name = STROKE::GET::mode(::deck);
  const auto gesture = STROKE::GET::gesture(::deck);
  std::cout << std::fixed << std::setprecision(1) << "stroke " << label
            << ": mode=" << (name.empty() ? "-" : name);
  if (gesture.opened) std::cout << " opened";
  if (gesture.standing) std::cout << " standing";
  if (gesture.closed) std::cout << " closed";
  if (gesture.dragged) std::cout << " dragged";
  if (gesture.control) std::cout << " control";
  if (gesture.shift) std::cout << " shift";
  if (gesture.alt) std::cout << " alt";
  std::cout << " from=";
  ::where(gesture.from);
  std::cout << " to=";
  ::where(gesture.to);
  std::cout << " laid=[";
  STRING::Hot comma = "";
  for (const auto &cell : STROKE::GET::laid(::deck)) {
    std::cout << comma << cell.across << "," << cell.down;
    comma = " ";
  }
  std::cout << "]" << std::endl;
}

void selected(STRING::Hot label) {
  std::cout << "stroke " << label << ": selected=[";
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

}  // namespace

Status drawn() {
  const String layout = LAYOUT;
  rescale(1.0f);
  ::deck = GUI::load("stroke", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("board load", ::deck != GUI::NONE);
  status |=
    check("undesignated refuses", STROKE::draw(::deck, ::pencil()) != 0);
  status |= check("adopt", GUI::SAC::adopt({String(STROKE::NAME)}) == 0);
  status |= check(
    "a lattice of no size",
    STROKE::draw(::deck, {"deck", "pencil", {0.0f, SIDE}}) != 0);
  status |= check("draw", STROKE::draw(::deck, ::pencil()) == 0);
  status |= check("place", GUI::place(::deck, WHOLE) == 0);
  ::at(0.0f, 0.0f, false);
  ::look("rest");
  status |= check(
    "select", GUI::NGA::set(::deck, "box", GUI::NGA::Selected{true}) == 0);
  ::at(150.0f, 150.0f, true);
  ::look("press");
  status |= check("opened", STROKE::GET::gesture(::deck).opened);
  status |= check("a cell laid", STROKE::GET::laid(::deck).size() == 1);
  ::at(210.0f, 190.0f, true);
  ::look("pull");
  status |= check("dragged", STROKE::GET::gesture(::deck).dragged);
  status |= check("walked", STROKE::GET::laid(::deck).size() == 3);
  ::at(150.0f, 150.0f, true);
  ::look("back");
  ::at(150.0f, 150.0f, false);
  ::look("closed");
  status |= check("closed", STROKE::GET::gesture(::deck).closed);
  ::look("after");
  status |= check("one frame", !STROKE::GET::gesture(::deck).closed);
  ::selected("claimed");
  status |=
    check("the selection stands", GUI::NGA::GET::selected(::deck, "box"));
  ::at(150.0f, 150.0f, true);
  ::look("still");
  ::at(155.0f, 155.0f, false);
  ::look("click");
  status |= check("a click, not a drag", !STROKE::GET::gesture(::deck).dragged);
  chord(true, false, false);
  ::at(150.0f, 150.0f, true);
  ::look("chorded");
  status |= check("the chord latched", STROKE::GET::gesture(::deck).control);
  chord(false, false, false);
  ::at(210.0f, 190.0f, true);
  ::look("pulling");
  status |= check(
    "held for the span", STROKE::GET::gesture(::deck).control &&
                           STROKE::GET::gesture(::deck).dragged);
  ::at(210.0f, 190.0f, false);
  ::look("chord closed");
  status |= check("closed chorded", STROKE::GET::gesture(::deck).control);
  ::at(150.0f, 150.0f, true);
  ::look("plain");
  status |= check("no chord", !STROKE::GET::gesture(::deck).control);
  ::at(150.0f, 150.0f, false);
  ::look("plain closed");
  ::at(70.0f, 55.0f, true);
  ::look("on the box");
  status |= check("not the pencil's", !STROKE::GET::gesture(::deck).standing);
  ::at(70.0f, 55.0f, false);
  ::look("let go");
  status |= check(
    "over the board", STROKE::draw(::deck, ::pencil(STROKE::Mode::BOARD)) == 0);
  ::at(70.0f, 55.0f, true);
  ::look("over the box");
  status |= check("the pencil's now", STROKE::GET::gesture(::deck).standing);
  ::at(70.0f, 55.0f, false);
  ::look("box closed");
  status |= check("stand down", STROKE::draw(::deck, {}) == 0);
  status |= check("no mode", STROKE::GET::mode(::deck).empty());
  ::at(150.0f, 150.0f, true);
  ::look("banding");
  ::at(210.0f, 190.0f, true);
  ::look("stretching");
  ::at(210.0f, 190.0f, false);
  ::look("down");
  ::selected("banded");
  status |= check("re-adopt", GUI::SAC::adopt({}) == 0);
  status |=
    check("the door closes again", STROKE::draw(::deck, ::pencil()) != 0);
  return status;
}
