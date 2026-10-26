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
#include <island/gui/backend/selection.hpp>
#include <test/gui/gui.internal.hpp>

#if SR_GUI_BACKEND == SR_NATIVE
#include <island/graphics/backend/resources.hpp>
#include <island/gui/backend/native/kinds.internal.hpp>
#include <island/gui/backend/native/native.hpp>

namespace {

constexpr Float SIDE = 400.0f;
constexpr STRING::Hot CELLS[] = {"door", "gain"};

auto name(GUI::Event::Kind kind) -> STRING::Hot {
  static constexpr STRING::Hot NAMES[] = {
    "entered",   "exited", "pressed",   "clicked", "selected",
    "activated", "edited", "committed", "dialled", "moved"};
  return NAMES[kind];
}

void rim(GUI::Handle document, STRING::Hot id) {
  Whole node = GUI::PARENTLESS;
  const auto &chrome = GUI::BACKEND::NATIVE::mirror(document);
  if (GUI::find(document, id, node) != 0 || !chrome.slots[node].shown) return;
  const auto &north = GFX::BUFFERS::GET::data<GUI::Element>(
    chrome.buffer)[chrome.slots[node].first + 1];
  std::cout << " " << id << "=" << north.x << "," << north.y << " " << north.w
            << "x" << north.h << " ink=" << north.color.r << ","
            << north.color.g << "," << north.color.b;
}

}  // namespace

void RING::look(GUI::Handle document, STRING::Hot leg) {
  GUI::poll();
  GFX::Pass pass = {.viewport = {0.0f, 0.0f, ::SIDE, ::SIDE}};
  GUI::flush(document, pass);
  const auto seat = GUI::GET::editing(document);
  std::cout << std::fixed << std::setprecision(2) << "reach " << leg
            << ": seat=" << (seat.empty() ? "-" : seat)
            << " seated=" << GUI::GET::seated(document)
            << " cursor=" << GUI::GET::cursor(document, "menu") << " events=[";
  STRING::Hot comma = "";
  for (const auto &event : GUI::GET::events(document)) {
    std::cout << comma << event.id << " " << ::name(event.kind);
    comma = ", ";
  }
  std::cout << "] rims";
  for (const auto cell : ::CELLS) ::rim(document, cell);
  std::cout << std::endl;
}

#endif
