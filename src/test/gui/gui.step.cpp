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

GUI::Handle deck = GUI::NONE;
GUI::Handle spare = GUI::NONE;

static Whole written = 0;

static auto name(GUI::Event::Kind kind) -> STRING::Hot {
  static constexpr STRING::Hot WORDS[] = {
    "entered", "exited",    "pressed", "clicked", "selected", "activated",
    "edited",  "committed", "dialled", "moved",   "joined",   "cut"};
  return WORDS[kind];
}

static Status handles() {
  using namespace GUI;
  Status status = NODES::create(deck, "sheet", "grip", "split");
  status |= set(deck, "split", Position{140.0f, 140.0f});
  status |= set(deck, "split", Extent{40.0f, 10.0f});
  status |= NODES::create(deck, "sheet", "grip", "rail");
  status |= set(deck, "rail", Position{40.0f, 190.0f});
  status |= set(deck, "rail", Extent{10.0f, 30.0f});
  status |= set(deck, "rail", Axis{true});
  status |= NODES::create(deck, "sheet", "grip", "rover");
  status |= set(deck, "rover", Position{240.0f, 140.0f});
  status |= set(deck, "rover", Extent{20.0f, 20.0f});
  return status | set(deck, "rover", Axis{.free = true});
}

void step(STRING::Hot name) {
  GUI::poll();
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::flush(deck, pass);
  GUI::flush(spare, pass);
  std::cout << "step " << name << ": hover=";
  const auto hover = GUI::GET::hover(deck);
  std::cout << (hover.empty() ? "-" : hover) << " events=[";
  STRING::Hot comma = "";
  for (const auto &event : GUI::GET::events(deck)) {
    std::cout << comma << event.id << " " << ::name(event.kind);
    comma = ", ";
  }
  const auto writes = GUI::GET::writes();
  std::cout << "] rewrites=" << writes - written << std::endl;
  written = writes;
}

Status grow() {
  using namespace GUI;
  deck = create("deck");
  Status status = NODES::create(deck, NODES::ROOT, "panel", "sheet");
  status |= set(deck, "sheet", Extent{320.0f, 240.0f});
  status |= NODES::create(deck, "sheet", "button", "corner");
  status |= set(deck, "corner", Extent{40.0f, 20.0f});
  status |= NODES::create(deck, "sheet", "button", "first");
  status |= set(deck, "first", Position{40.0f, 40.0f});
  status |= set(deck, "first", Extent{80.0f, 30.0f});
  status |= NODES::create(deck, "first", "label", "tag");
  status |= set(deck, "tag", Extent{80.0f, 30.0f});
  status |= NODES::create(deck, "sheet", "button", "second");
  status |= set(deck, "second", Position{100.0f, 60.0f});
  status |= set(deck, "second", Extent{80.0f, 30.0f});
  status |= NODES::create(deck, "sheet", "button", "ghost");
  status |= set(deck, "ghost", Position{200.0f, 40.0f});
  status |= set(deck, "ghost", Extent{80.0f, 30.0f});
  status |= set(deck, "ghost", Visibility{false});
  status |= ::handles();
  spare = create("spare");
  status |= NODES::create(spare, NODES::ROOT, "button", "lone");
  status |= set(spare, "lone", Extent{320.0f, 240.0f});
  return status;
}
