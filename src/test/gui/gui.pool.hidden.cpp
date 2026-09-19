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
constexpr Float SIDE = 200.0f;
constexpr Whole ROWS = 3;

void stack(GUI::Handle page, STRING::Hot id, Float x) {
  using namespace GUI;
  NODES::create(page, "sheet", "panel", id);
  set(page, id, Position{x, 0.0f});
  set(page, id, Extent{SIDE, SIDE});
  for (Whole row = 0; row < ROWS; row += 1) {
    const String name = String(id) + std::to_string(row);
    NODES::create(page, id, "button", name.c_str());
    set(page, name.c_str(), Position{10.0f, 10.0f + 40.0f * row});
    set(page, name.c_str(), Extent{120.0f, 30.0f});
  }
}

void look(GUI::Handle page, STRING::Hot name, STRING::Hot id) {
  const auto at = GUI::GET::origin(page, id);
  const auto box = GUI::GET::measured(page, id);
  std::cout << "pool " << name << ": " << id << " at=" << at.x << "," << at.y
            << " box=" << box.w << "x" << box.h << std::endl;
}

auto flushed(GUI::Handle page) -> Whole {
  const Whole mark = GUI::GET::writes();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(page, pass);
  return GUI::GET::writes() - mark;
}

}  // namespace

Status hidden() {
  using namespace GUI;
  rescale(1.0f);
  Handle page = create("poolpage");
  NODES::create(page, NODES::ROOT, "panel", "sheet");
  set(page, "sheet", Extent{WIDE, TALL});
  ::stack(page, "left", 0.0f);
  ::stack(page, "right", SIDE + 40.0f);
  Status status = check("place", place(page, WHOLE) == 0);
  const Whole both = ::flushed(page);
  ::look(page, "stood", "right0");
  status |= check("put away", set(page, "right", Visibility{false}) == 0);
  const Whole one = ::flushed(page);
  std::cout << "pool writes: both=" << both << " one=" << one << std::endl;
  status |= check("the hidden stack wrote nothing", one < both);
  ::look(page, "hidden", "right");
  ::look(page, "under", "right0");
  status |= check(
    "a hidden row keeps its parent's box",
    GET::origin(page, "right0") == GET::origin(page, "right"));
  return status;
}
