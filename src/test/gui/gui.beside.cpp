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

constexpr Float WIDE = 640.0f;
constexpr Float TALL = 480.0f;
constexpr Float AIR = 6.0f;

void look(GUI::Handle document, STRING::Hot name, STRING::Hot id) {
  const auto at = GUI::GET::origin(document, id);
  const auto box = GUI::GET::measured(document, id);
  std::cout << "beside " << name << ": " << id << " at=" << at.x << "," << at.y
            << " box=" << box.w << "x" << box.h << std::endl;
}

void gap(GUI::Handle document, STRING::Hot name, STRING::Hot id) {
  const auto room = GUI::GET::room(document, id);
  std::cout << "beside " << name << ": " << id << " room=" << room.west << ","
            << room.east << "," << room.north << "," << room.south << std::endl;
}

void compile(GUI::Handle document) {
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(document, pass);
}

auto grown() -> GUI::Handle {
  using namespace GUI;
  Handle document = create("seatdeck");
  NODES::create(document, NODES::ROOT, "panel", "sheet");
  set(document, "sheet", Extent{WIDE, TALL});
  NODES::create(document, "sheet", "button", "door");
  set(document, "door", Position{200.0f, 100.0f});
  set(document, "door", Extent{120.0f, 40.0f});
  NODES::create(document, "sheet", "panel", "plate");
  set(document, "plate", Extent{160.0f, 80.0f});
  set(document, "plate", Seat{"door", BELOW, AIR});
  NODES::create(document, "sheet", "panel", "flush");
  set(document, "flush", Extent{200.0f, 30.0f});
  set(document, "flush", NORTHEAST);
  set(document, "flush", Seat{"door", ABOVE, AIR});
  NODES::create(document, "sheet", "panel", "pane");
  set(document, "pane", Extent{90.0f, 60.0f});
  set(document, "pane", Seat{"door", EAST, AIR});
  return document;
}

}  // namespace

Status beside() {
  using namespace GUI;
  rescale(1.0f);
  Handle document = ::grown();
  Status status = check("build", document != NONE);
  status |= check("place", place(document, WHOLE) == 0);
  ::compile(document);
  ::look(document, "door", "door");
  ::look(document, "below", "plate");
  ::look(document, "above", "flush");
  ::look(document, "east", "pane");
  ::gap(document, "room", "plate");
  status |= check(
    "the seat reads back",
    GET::seat(document, "plate") == Seat{"door", BELOW, AIR});
  status |= check(
    "no seat is the zero seat", GET::seat(document, "door").beside.empty());
  set(document, "door", Position{40.0f, 100.0f});
  ::compile(document);
  ::look(document, "carried", "plate");
  ::look(document, "carried", "flush");
  set(document, "door", Position{40.0f, 430.0f});
  ::compile(document);
  ::look(document, "foot", "door");
  ::look(document, "flipped", "plate");
  status |= check(
    "the flipped plate stands whole",
    GET::origin(document, "plate").y >= 0.0f &&
      GET::origin(document, "plate").y + GET::measured(document, "plate").h <=
        TALL);
  set(document, "plate", Extent{160.0f, 460.0f});
  ::compile(document);
  ::look(document, "clamped", "plate");
  set(document, "plate", Extent{160.0f, 80.0f});
  set(document, "door", Position{560.0f, 100.0f});
  ::compile(document);
  ::look(document, "wall", "door");
  ::look(document, "west", "pane");
  ::look(document, "fitted", "flush");
  status |= check(
    "a node cannot seat onto itself",
    set(document, "plate", Seat{"plate", BELOW, AIR}) != 0);
  status |= check(
    "an unknown referent refuses",
    set(document, "plate", Seat{"ghost", BELOW, AIR}) != 0);
  status |= check(
    "the ring's last edge refuses",
    set(document, "door", Seat{"plate", BELOW, AIR}) != 0);
  status |= check(
    "the refused seat left the plate where it was",
    GET::seat(document, "plate") == Seat{"door", BELOW, AIR});
  status |= check("the seat clears", set(document, "plate", Seat{}) == 0);
  ::compile(document);
  ::look(document, "freed", "plate");
  return status;
}
