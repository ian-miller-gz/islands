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
#include <island/gui/tip.hpp>
#include <test/gui/gui.internal.hpp>

namespace TIP = GUI::TIP;

namespace {

constexpr Float STEP = 8.0f;

void look(GUI::Handle document, STRING::Hot name) {
  const auto at = GUI::GET::position(document, TIP::PLATE);
  const auto box = GUI::GET::extent(document, TIP::PLATE);
  std::cout << "tip " << name
            << ": plate=" << GUI::GET::visibility(document, TIP::PLATE)
            << " label='" << GUI::GET::text(document, TIP::LABEL)
            << "' at=" << at.x << "," << at.y << " box=" << box.w << "x"
            << box.h << std::endl;
}

void settle(Whole count) {
  for (Whole poll = 0; poll < count; poll += 1) GUI::poll();
}

auto grown() -> GUI::Handle {
  using namespace GUI;
  Handle document = create("tipdeck");
  NODES::create(document, NODES::ROOT, "panel", "sheet");
  set(document, "sheet", Extent{640.0f, 480.0f});
  NODES::create(document, "sheet", "button", "door");
  set(document, "door", Position{60.0f, 50.0f});
  set(document, "door", Extent{120.0f, 40.0f});
  NODES::create(document, "sheet", "label", "door.hint");
  set(document, "door.hint", Text{"the rack view - the armed instrument"});
  set(document, "door.hint", Visibility{false});
  NODES::create(document, "sheet", "button", "plain");
  set(document, "plain", Position{60.0f, 150.0f});
  set(document, "plain", Extent{120.0f, 40.0f});
  NODES::create(document, "sheet", "button", "row");
  set(document, "row", Position{60.0f, 250.0f});
  set(document, "row", Extent{300.0f, 40.0f});
  NODES::create(document, "row", "label", "row.name");
  set(document, "row.name", Extent{150.0f, 40.0f});
  NODES::create(document, "row", "label", "row.value");
  set(document, "row.value", Position{150.0f, 0.0f});
  set(document, "row.value", Extent{150.0f, 40.0f});
  NODES::create(document, "sheet", "button", "edge");
  set(document, "edge", Position{560.0f, 440.0f});
  set(document, "edge", Extent{80.0f, 40.0f});
  NODES::create(document, "sheet", "label", "edge.hint");
  set(document, "edge.hint", Text{String(80, 'a') + " " + String(80, 'b')});
  set(document, "edge.hint", Visibility{false});
  NODES::create(document, "sheet", "panel", "tip");
  set(document, "tip", Visibility{false});
  NODES::create(document, "tip", "label", "tip.label");
  set(document, "tip.label", Position{8.0f, 6.0f});
  set(document, "tip.label", Size{16.0f});
  return document;
}

void compile(GUI::Handle document) {
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(document, pass);
}

}  // namespace

Status tip() {
  using namespace GUI;
  SAC::adopt({TIP::NAME});
  rescale(1.0f);
  metrics(0.0f, STEP);
  Handle document = ::grown();
  Status status = check("build", document != NONE);
  status |= check("place", place(document, WHOLE) == 0);
  ::compile(document);
  aim(120.0f, 70.0f);
  settle(1);
  ::look(document, "wake");
  settle(TIP::REST - 1);
  ::look(document, "almost");
  settle(1);
  ::look(document, "rest");
  status |= check("hint rises", GET::visibility(document, TIP::PLATE));
  aim(120.0f, 170.0f);
  settle(1);
  ::look(document, "slide");
  settle(TIP::REST + 1);
  ::look(document, "stand");
  TIP::note(document, "plain", "the silent door's noted line");
  settle(TIP::REST + 1);
  ::look(document, "noted");
  TIP::note(document, "plain", "");
  settle(1);
  ::look(document, "forgot");
  TIP::note(document, "row.name", "alpha - the row's full name");
  TIP::note(document, "row.value", "1.0009765625");
  aim(320.0f, 270.0f);
  settle(TIP::REST + 1);
  ::look(document, "cell");
  status |= check(
    "cell answers its figure",
    GET::text(document, TIP::LABEL) == "1.0009765625");
  aim(150.0f, 270.0f);
  settle(1);
  ::look(document, "column");
  settle(TIP::REST);
  ::look(document, "name");
  aim(600.0f, 455.0f);
  settle(TIP::REST + 1);
  ::look(document, "corner");
  aim(700.0f, 500.0f);
  settle(1);
  ::look(document, "away");
  status |= check("away hides", !GET::visibility(document, TIP::PLATE));
  remove(document);
  Handle bare = create("bare");
  NODES::create(bare, NODES::ROOT, "button", "lone");
  set(bare, "lone", Extent{640.0f, 480.0f});
  NODES::create(bare, "lone", "label", "lone.hint");
  set(bare, "lone.hint", Text{"a hint no plate will carry"});
  set(bare, "lone.hint", Visibility{false});
  status |= check("place bare", place(bare, WHOLE) == 0);
  ::compile(bare);
  aim(320.0f, 240.0f);
  settle(TIP::REST + 1);
  status |= check("no plate never raises", !GET::visibility(bare, TIP::PLATE));
  return status;
}
