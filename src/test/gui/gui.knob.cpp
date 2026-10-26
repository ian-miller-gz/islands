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
#include <string>
#include <test/gui/gui.internal.hpp>

namespace {

using KNOB::Step;

constexpr STRING::Hot LAYOUT =
  "dial knob x=8 y=40 w=200 h=20 least=0 most=1 resting=0.3 value=0.3 "
  "graphic=dial text=0.3\n"
  "dial notch x=8 y=80 w=200 h=20 least=0 most=1 steps=4 resting=0.3 "
  "value=0.3 graphic=dial\n"
  "dial slide x=8 y=120 w=200 h=20 least=0 most=1 resting=0.3 value=0.3 "
  "graphic=bar text=0.3\n"
  "dial pan x=8 y=160 w=200 h=20 least=-1 most=1 value=0.5 graphic=dial "
  "track=#c8b89aff\n";

constexpr Step TYPE[] = {
  {100, 50, true, "1 press"}, {100, 50, false, "1 click"}};
constexpr Step TURN[] = {
  {100, 50, true, "2 press"},     {100, 10, true, "2 up"},
  {100, 10, false, "2 release"},  {100, 50, true, "3 press"},
  {140, 50, true, "3 across"},    {140, 110, true, "3 down"},
  {140, 110, false, "3 release"}, {100, 50, true, "4 first"},
  {100, 50, false, "4 once"},     {100, 50, true, "4 again"},
  {100, 50, false, "4 rested"}};
constexpr Step SHORT[] = {
  {100, 90, true, "5 press"}, {100, 60, true, "5 short"}};
constexpr Step PAST[] = {
  {100, 30, true, "5 past"},
  {100, 30, false, "5 release"},
  {58, 130, true, "6 press"},
  {58, 130, false, "6 release"}};

constexpr Float LINE = 12.0f;
constexpr Float ADVANCE = 8.0f;

GUI::Handle bench = GUI::NONE;

void type() {
  KNOB::walk(::bench, ::TYPE);
  typed(
    {{INPUT::KEYS::TEXT, '0'},
     {INPUT::KEYS::TEXT, '.'},
     {INPUT::KEYS::TEXT, '8'},
     {INPUT::KEYS::ENTER, 0}});
  KNOB::look(::bench, "1 entered");
  if (GUI::GET::committed(::bench, "knob")) {
    const Float number = std::stof(GUI::GET::text(::bench, "knob"));
    GUI::set(::bench, "knob", GUI::Value{number});
  }
  KNOB::look(::bench, "1 typed");
}

void gather() {
  const Float before = GUI::GET::value(::bench, "notch");
  KNOB::walk(::bench, ::SHORT);
  const auto &pointer = GUI::STATE::documents[::bench].pointer;
  if (pointer.dragged && GUI::GET::value(::bench, "notch") == before)
    std::cout << "gathered notch" << std::endl;
  KNOB::walk(::bench, ::PAST);
}

}  // namespace

Status knob() {
  const String layout = LAYOUT;
  metrics(::LINE, ::ADVANCE);
  ::bench = GUI::load("knob", Vector<Byte>(layout.begin(), layout.end()));
  Status status = check("knob load", ::bench != GUI::NONE);
  status |= check("place", GUI::place(::bench, WHOLE) == 0);
  status |= check("focus", GUI::focus(::bench, true) == 0);
  status |= check(
    "graphic", GUI::GET::dial(::bench, "knob").graphic == GUI::Dial::DIAL);
  status |= KNOB::measure(::bench) | KNOB::emit(::bench);
  ::type();
  KNOB::walk(::bench, ::TURN);
  status |= check("unseat", GUI::edit(::bench, "") == 0);
  ::gather();
  return status;
}
