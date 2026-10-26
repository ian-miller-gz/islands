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
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/backend/resources.hpp>
#include <island/gui/backend/native/kinds.internal.hpp>
#include <island/gui/backend/native/native.hpp>

namespace {

constexpr STRING::Hot LAYOUT =
  "stroke line to=160,160\n"
  "stroke rise to=160,160 shape=rise\n"
  "stroke fall to=160,160 shape=fall\n"
  "stroke ease to=160,160 shape=ease\n"
  "stroke hold to=160,160 shape=hold\n"
  "stroke moved x=20 y=40 to=160,160\n"
  "stroke bent to=160,160 shape=rise bend=1\n"
  "stroke still to=0,0\n"
  "graph deck x=0 y=0 w=400 h=400 zoom=2\n"
  "  stroke aboard x=10 y=10 to=80,80 border=3\n";

constexpr Float SIDE = 400.0f;

GUI::Handle bench = GUI::NONE;

void pieces(STRING::Hot id) {
  Whole node = GUI::PARENTLESS;
  const auto &chrome = GUI::BACKEND::NATIVE::mirror(::bench);
  if (GUI::find(::bench, id, node) != 0) return;
  const auto &slot = chrome.slots[node];
  const auto *elements =
    GFX::BUFFERS::GET::data<GUI::Element>(chrome.buffer) + slot.first;
  std::cout << std::fixed << std::setprecision(2) << "strokes " << id
            << ": at=" << slot.x << "," << slot.y
            << " shape=" << GUI::GET::shape(::bench, id).run
            << " bend=" << GUI::GET::bend(::bench, id)
            << " to=" << GUI::GET::ends(::bench, id).to.x << ","
            << GUI::GET::ends(::bench, id).to.y << std::endl;
  for (Whole at = 0; at < GUI::KINDS::SEGMENTS; at += 1)
    std::cout << "  " << at << " " << elements[at].x << "," << elements[at].y
              << " " << elements[at].w << "x" << elements[at].h << std::endl;
}

}  // namespace

Status laid() {
  const String layout = LAYOUT;
  ::bench = GUI::load("strokes", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("strokes load", ::bench != GUI::NONE);
  GFX::Pass pass = {.viewport = {0.0f, 0.0f, ::SIDE, ::SIDE}};
  GUI::flush(::bench, pass);
  for (STRING::Hot id :
       {"line", "rise", "fall", "ease", "hold", "moved", "bent", "still",
        "aboard"})
    ::pieces(id);
  const String bogus = "stroke odd to=160,160 shape=curl\n";
  status |= check(
    "refuse unknown shape",
    GUI::load("odd", Vector<Byte>(bogus.begin(), bogus.end())) == GUI::NONE);
  const String lone = "stroke lone to=160\n";
  status |= check(
    "refuse half an end",
    GUI::load("lone", Vector<Byte>(lone.begin(), lone.end())) == GUI::NONE);
  return status;
}

#else

Status laid() {
  std::cout << "strokes: the native backend draws the pieces (SR_RMLUI)"
            << std::endl;
  return 1;
}

#endif
