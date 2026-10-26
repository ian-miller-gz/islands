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
#include <island/window/backend.hpp>
#include <test/gui/gui.internal.hpp>

namespace {

constexpr STRING::Hot WORDS[] = {"arrow", "ibeam", "across", "down"};

constexpr Float OFF = 60.0f;

void look(STRING::Hot name) {
  step(name);
  std::cout << "shape " << ::WORDS[WINDOW::GET::shape()] << std::endl;
}

Status pull(STRING::Hot id, Float x, Float y, WINDOW::Shape held) {
  const Status pressed = GUI::press(deck, id, 0.0f, 0.0f);
  ::look(id);
  Status status = check("grip shape", !pressed && WINDOW::GET::shape() == held);
  const Status pulled = GUI::travel(deck, x, y);
  ::look("held");
  status |= check("held shape", !pulled && WINDOW::GET::shape() == held);
  const Status released = GUI::release(deck, 0.0f, 0.0f);
  ::look("off");
  return status |
         check(
           "rest shape", !released && WINDOW::GET::shape() == WINDOW::ARROW);
}

}  // namespace

Status shaped() {
  Status status = ::pull("split", ::OFF, 0.0f, WINDOW::DOWN);
  return status | ::pull("rail", 0.0f, -::OFF, WINDOW::ACROSS);
}
