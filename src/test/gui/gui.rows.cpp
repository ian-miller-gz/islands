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
#include <island/gui/rows.hpp>
#include <test/gui/gui.internal.hpp>

namespace {
constexpr Float MARGIN = 10.0f;
constexpr Float GAP = 4.0f;
constexpr Float BAND = 20.0f;
constexpr Float CABLE = 5.0f;
}  // namespace

Status stacked() {
  using namespace GUI::SAC;
  const Vector<ROWS::Row> wanted = {
    {::BAND, true}, {::CABLE, false}, {::BAND, true}, {::CABLE, false}};
  Status status = check(
    "undesignated seats no row", ROWS::stack(wanted, ::MARGIN, ::GAP).empty());
  status |= check("the name adopts", adopt({String(ROWS::NAME)}) == 0);
  const Vector<ROWS::Rail> rails = ROWS::stack(wanted, ::MARGIN, ::GAP);
  status |= check("every row is seated", rails.size() == wanted.size());
  status |= check(
    "the first row opens at the margin and is charged no gap",
    rails[0].top == 10.0f && rails[0].depth == ::BAND);
  status |= check("the next stands on the sum above it", rails[1].top == 30.0f);
  status |= check("a parting charges the gap", rails[2].top == 39.0f);
  status |= check("and the row behind it does not", rails[3].top == 59.0f);
  status |= check(
    "the floor is the last row's far edge",
    ROWS::deep(rails, ::MARGIN) == 64.0f);
  status |= check(
    "an empty stack is the margin deep", ROWS::deep({}, ::MARGIN) == ::MARGIN);
  status |= check("a point inside a row names it", ROWS::at(rails, 31.0f) == 1);
  status |=
    check("a row's own top edge is inside it", ROWS::at(rails, 39.0f) == 2);
  status |=
    check("and its far edge is the next row's", ROWS::at(rails, 59.0f) == 3);
  status |=
    check("the margin names no row", ROWS::at(rails, 5.0f) == ROWS::NONE);
  status |= check("a gap names no row", ROWS::at(rails, 36.0f) == ROWS::NONE);
  status |=
    check("past the floor names no row", ROWS::at(rails, 99.0f) == ROWS::NONE);
  status |= check(
    "the asked reach is the window's far edge",
    ROWS::reach(20.0f, 100.0f) == 120.0f);
  status |= check(
    "the high-water reach grows with its spare",
    ROWS::reach(100.0f, 20.0f, 100.0f, 0.5f) == 170.0f);
  status |= check(
    "and never comes back in",
    ROWS::reach(170.0f, 0.0f, 100.0f, 0.5f) == 170.0f);
  status |= check("re-adoption replaces whole", adopt({}) == 0);
  status |= check(
    "the door closed again", ROWS::stack(wanted, ::MARGIN, ::GAP).empty());
  return status;
}
