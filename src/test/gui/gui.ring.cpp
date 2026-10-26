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
#include <island/gui/backend/selection.hpp>
#include <test/gui/gui.internal.hpp>

#if SR_GUI_BACKEND == SR_NATIVE

namespace {

constexpr STRING::Hot LAYOUT =
  "field name x=8 y=8 w=100 h=20\n"
  "button door x=8 y=40 w=100 h=20 border=1 glow=#ff8000ff\n"
  "panel shut visible=false\n"
  "  button ghost x=8 y=70 w=100 h=20\n"
  "dial gain x=8 y=100 w=200 h=20 least=0 most=1 value=0.5\n"
  "list menu x=8 y=130 w=200 h=58 rows=4\n";

GUI::Handle bench = GUI::NONE;

void look(STRING::Hot leg) { RING::look(::bench, leg); }

void press(INPUT::KEYS::Action action) { typed({{action, 0, false, false}}); }

void tab() { typed({{INPUT::KEYS::TEXT, INPUT::KEYS::TAB}}); }

}  // namespace

Status reached() {
  const String layout = LAYOUT;
  ::bench = GUI::load("reach", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("reach load", ::bench != GUI::NONE);
  status |= check("focus", GUI::focus(::bench, true) == 0);
  status |= check("field seated", GUI::edit(::bench, "name") == 0);
  ::look("field");
  ::tab();
  ::look("door");
  ::press(INPUT::KEYS::ENTER);
  ::look("pressed");
  status |= check("clicked answers once", GUI::GET::clicked(::bench, "door"));
  status |= check("clicked consumed", !GUI::GET::clicked(::bench, "door"));
  ::tab();
  ::look("dial");
  ::tab();
  ::look("wraps");
  status |= check("button seated", GUI::edit(::bench, "door") == 0);
  status |= check("never seated", !GUI::GET::seated(::bench));
  ::press(INPUT::KEYS::DOWN);
  ::look("hands");
  status |= check("list refused", GUI::edit(::bench, "menu") != 0);
  status |= check("reach again", GUI::edit(::bench, "door", false) == 0);
  GUI::set(::bench, "door", GUI::Visibility{false});
  ::press(INPUT::KEYS::ENTER);
  ::look("hidden");
  return status;
}

#else

Status reached() {
  std::cout << "reach: rim drawn by native backend only" << std::endl;
  return 1;
}

#endif
