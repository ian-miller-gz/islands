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
  "  node box x=30 y=30 w=80 h=40 text=\"box\"\n"
  "  node rail x=4 y=30 w=24 h=40 pinned=across text=\"rail\"\n"
  "  node cap x=30 y=4 w=80 h=16 pinned=down text=\"cap\"\n";

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
  std::cout << std::fixed << std::setprecision(1) << "pinned " << name << ":";
  ::where("box");
  ::where("rail");
  ::where("cap");
  std::cout << " hover=" << (over.empty() ? "-" : over) << std::endl;
}

}  // namespace

Status pinned() {
  const String layout = LAYOUT;
  rescale(2.0f);
  ::bench = GUI::load("pinned", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("pinned load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  using Axis = GUI::NGA::Pinned::Axis;
  status |= check(
    "across read", GUI::NGA::GET::pinned(::bench, "rail").axis == Axis::ACROSS);
  status |= check(
    "down read", GUI::NGA::GET::pinned(::bench, "cap").axis == Axis::DOWN);
  status |= check(
    "none read", GUI::NGA::GET::pinned(::bench, "box").axis == Axis::NONE);
  aim(0.0f, 0.0f);
  hold(false);
  ::look("rest");
  status |= check(
    "pan", GUI::NGA::set(::bench, "deck", GUI::NGA::Pan{20.0f, 10.0f}) == 0);
  ::look("panned");
  status |=
    check("zoom", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f}) == 0);
  ::look("zoomed");
  status |= check(
    "stretch", GUI::NGA::set(::bench, "deck", GUI::NGA::Zoom{2.0f, 1.0f}) == 0);
  ::look("stretched");
  aim(52.0f, 90.0f);
  hold(false);
  ::look("over");
  status |= check("picked where it stands", GUI::GET::hover(::bench) == "rail");
  status |=
    check("unpin", GUI::NGA::set(::bench, "rail", GUI::NGA::Pinned{}) == 0);
  ::look("unpinned");
  status |= check(
    "repin",
    GUI::NGA::set(::bench, "rail", GUI::NGA::Pinned{Axis::ACROSS}) == 0);
  ::look("repinned");
  const String bad =
    "graph b x=0 y=0 w=10 h=10\n  node n x=0 y=0 w=5 h=5 pinned=sideways\n";
  status |= check(
    "refuse a stranger name",
    GUI::load("bad", Vector<Byte>(bad.begin(), bad.end())) == GUI::NONE);
  return status;
}
