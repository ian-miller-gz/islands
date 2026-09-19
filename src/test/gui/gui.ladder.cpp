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
#include <island/gui/ladder.hpp>
#include <test/gui/gui.internal.hpp>

namespace {
constexpr Whole BAR = 4;

String barred(Whole at) {
  return at % ::BAR == 0 ? std::to_string(at / ::BAR + 1) : String();
}
}  // namespace

Status laddered() {
  using namespace GUI::SAC;
  const Vector<LADDER::Rung> rungs = {
    {1, "beat", ::barred}, {4, "bar", ::barred}, {16, "minute", nullptr}};
  Status status = check(
    "undesignated dresses nothing",
    LADDER::dress(rungs[0], ::BAR, 0.0f, 10.0f).empty());
  status |= check("the name adopts", adopt({String(LADDER::NAME)}) == 0);
  status |= check(
    "the climb answers the first rung a crowd wide",
    String(LADDER::climb(rungs, 6.0f, 20.0f).name) == "bar");
  status |= check(
    "and the coarsest when none is",
    String(LADDER::climb(rungs, 1.0f, 20.0f).name) == "minute");
  status |= check(
    "an empty ladder has no rung", LADDER::climb({}, 1.0f, 20.0f).span == 0);
  status |= check(
    "the doubling climbs to the crowd",
    LADDER::climb(1, 2, 6.0f, 20.0f, 100.0f) == 4);
  status |= check(
    "and stops at the window", LADDER::climb(1, 2, 0.1f, 20.0f, 8.0f) == 8);
  status |= check(
    "an unmeasured axis answers the finest",
    LADDER::climb(1, 2, 0.0f, 20.0f, 0.0f) == 1);
  const Vector<LADDER::Mark> marks =
    LADDER::dress({2, "beat", ::barred}, ::BAR, 0.0f, 6.0f);
  status |= check("a face's worth with the spare pair", marks.size() == 5);
  status |= check(
    "the bar mark opens and is numbered",
    marks[0].opens && marks[0].label == "1");
  status |= check(
    "the beat between says nothing", !marks[1].opens && marks[1].label.empty());
  status |= check("the second bar counts on", marks[2].label == "2");
  const Vector<LADDER::Mark> panned =
    LADDER::dress({2, "beat", ::barred}, ::BAR, -5.0f, 4.0f);
  status |= check(
    "a negative pan opens on a real mark",
    panned.size() == 4 && panned[0].at == -6);
  status |= check(
    "the bar under zero still opens", panned[1].opens && panned[1].at == -4);
  status |= check(
    "negative marks state nothing",
    panned[0].label.empty() && panned[1].label.empty());
  status |= check(
    "zero opens the first bar", panned[3].opens && panned[3].label == "1");
  status |= check(
    "a span of nothing has no mark",
    LADDER::dress({0, "", nullptr}, 0, 0.0f, 10.0f).empty());
  status |= check("re-adoption replaces whole", adopt({}) == 0);
  status |= check(
    "the door closed again",
    LADDER::dress(rungs[0], ::BAR, 0.0f, 10.0f).empty());
  return status;
}
