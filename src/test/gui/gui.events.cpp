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
#include <test/gui/gui.internal.hpp>

namespace {

constexpr Float SCALE = 2.0f;
constexpr GFX::Viewport BOXED = {160.0f, 120.0f, 320.0f, 240.0f};

Status across() {
  using namespace GUI;
  aim(120.0f, 100.0f);
  step("wake");
  step("enter");
  hold(true);
  step("press");
  hold(false);
  step("release");
  Status status = check("clicked answers once", GET::clicked(deck, "first"));
  status |= check("clicked consumed", !GET::clicked(deck, "first"));
  aim(220.0f, 130.0f);
  step("overlap");
  hold(true);
  step("seize");
  aim(60.0f, 300.0f);
  step("escape");
  hold(false);
  step("abandon");
  status |=
    check("release outside clicks nothing", !GET::clicked(deck, "second"));
  aim(420.0f, 100.0f);
  step("shade");
  return status;
}

Status collapsed() {
  using namespace GUI;
  aim(120.0f, 100.0f);
  step("prime");
  flick();
  step("flick");
  Status status = check("collapsed click lands", GET::clicked(deck, "first"));
  aim(420.0f, 300.0f);
  flick(120.0f, 100.0f);
  step("stray");
  status |= check("carried click lands", GET::clicked(deck, "first"));
  return status;
}

Status overlap() {
  using namespace GUI;
  Status status = check("re-place deck", place(deck, WHOLE) == 0);
  status |= check("place spare", place(spare, WHOLE) == 0);
  aim(120.0f, 100.0f);
  step("share");
  status |= check("shared spare hovers", GET::hover(spare) == "lone");
  claim(deck, true);
  step("claim");
  status |= check("claimant still hovers", GET::hover(deck) == "first");
  status |= check("claimed-away spare rests", GET::hover(spare).empty());
  hold(true);
  step("under");
  status |= check("claimed-away spare no event", GET::events(spare).empty());
  hold(false);
  step("lift");
  status |= check("claimed-away spare no click", !GET::clicked(spare, "lone"));
  claim(deck, false);
  return status;
}

Status boxed() {
  using namespace GUI;
  Status status = check("re-place", place(deck, BOXED) == 0);
  aim(159.5f, 120.5f);
  step("outside");
  aim(160.5f, 120.5f);
  step("edge");
  hold(true);
  step("hold");
  hold(false);
  step("loose");
  status |= check("letterboxed click lands", GET::clicked(deck, "corner"));
  aim(600.0f, 400.0f);
  step("beyond");
  return status;
}

}  // namespace

Status events() {
  using namespace GUI;
  rescale(::SCALE);
  Status status = check("events build", grow() == 0);
  status |= check("refuse unknown place", place(NONE, WHOLE) != 0);
  status |= check("place", place(deck, WHOLE) == 0);
  status |= ::across();
  status |= ::collapsed();
  status |= ::boxed();
  status |= travel();
  status |= check("unplaced never hovers", GET::hover(spare).empty());
  status |= check("unplaced never clicks", !GET::clicked(spare, "lone"));
  status |= check("unplaced sees no events", GET::events(spare).empty());
  status |= ::overlap();
  return status | shadowed();
}
