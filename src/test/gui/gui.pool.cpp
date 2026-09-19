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

constexpr Whole MANY = 1000;

void look(GUI::Handle document, STRING::Hot name) {
  const auto pool = GUI::GET::nodes(document);
  std::cout << "pool " << name << ": live=" << pool.live
            << " dead=" << pool.dead << std::endl;
}

auto named(STRING::Hot mark, Whole row) -> String {
  return String(mark) + std::to_string(row);
}

void compose(GUI::Handle document, STRING::Hot mark) {
  for (Whole row = 0; row < MANY; row += 1)
    GUI::NODES::create(document, "deck", "label", ::named(mark, row).c_str());
}

void sweep(GUI::Handle document, STRING::Hot mark) {
  for (Whole row = 0; row < MANY; row += 1)
    GUI::NODES::remove(document, ::named(mark, row).c_str());
}

}  // namespace

Status pooled() {
  using namespace GUI;
  Handle document = create("pooldeck");
  Status status = check("build", document != NONE);
  status |=
    check("deck", NODES::create(document, NODES::ROOT, "panel", "deck") == 0);
  ::look(document, "empty");
  ::compose(document, "a");
  const Pool composed = GET::nodes(document);
  ::look(document, "composed");
  ::sweep(document, "a");
  ::look(document, "swept");
  ::compose(document, "b");
  ::look(document, "again");
  status |= check("the pool did not grow", GET::nodes(document) == composed);
  status |=
    check("hollow", NODES::remove(document, ::named("b", 0).c_str()) == 0);
  ::look(document, "hollowed");
  status |= check("raze", NODES::remove(document, "deck") == 0);
  ::look(document, "razed");
  status |= check("a stranger has no pool", GET::nodes(NONE) == Pool{});
  return status | hidden();
}
