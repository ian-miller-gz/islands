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
#include <island/graphics/backend/passes.hpp>
#include <test/gui/gui.internal.hpp>

namespace {

auto grown(GUI::Handle document, STRING::Hot id) -> Flag {
  Whole node = GUI::PARENTLESS;
  return GUI::find(document, id, node) == 0;
}

void beat(GUI::Handle document) {
  GFX::Pass pass = {.viewport = WHOLE};
  GUI::poll();
  GUI::flush(document, pass);
}

}  // namespace

Status thumb() {
  using namespace GUI;
  Handle document = create("thumb");
  Status status = check("create thumb document", document != NONE);
  for (STRING::Hot id : {"deep", "short"}) {
    status |= NODES::create(document, NODES::ROOT, "list", id);
    status |= set(document, id, Extent{100.0f, 100.0f});
    status |= set(document, id, Pitch{10.0f});
  }
  status |= set(document, "deep", Rows{40});
  status |= set(document, "short", Rows{4});
  GFX::Pass pass = {.viewport = WHOLE};
  flush(document, pass);
  ::beat(document);

  status |=
    check("a cropped list wears its mark", ::grown(document, "deep.scroll"));
  status |=
    check("a fitting list wears none", !::grown(document, "short.scroll"));

  const auto size = GET::extent(document, "deep.scroll");
  status |= check("the mark is two units wide", size.w == 2.0f);
  status |= check("the mark is the window's share", size.h == 25.0f);
  status |= check(
    "the mark rides the east rim",
    GET::anchor(document, "deep.scroll") == NORTHEAST);
  status |= check(
    "the mark starts at the head",
    GET::position(document, "deep.scroll").y == 0.0f);

  status |= set(document, "deep", Scroll{30});
  ::beat(document);
  status |= check(
    "the mark rides the window",
    GET::position(document, "deep.scroll").y == 75.0f);

  status |= set(document, "deep", Rows{4});
  ::beat(document);
  status |= check(
    "rows that fit retire the mark", !GET::visibility(document, "deep.scroll"));

  status |= NODES::create(document, NODES::ROOT, "list", "bare");
  status |= set(document, "bare", Extent{100.0f, 100.0f});
  status |= set(document, "bare", Pitch{10.0f});
  status |= set(document, "bare", Rows{40});
  status |= set(document, "bare", Scrolling{false});
  ::beat(document);
  status |=
    check("scroll=false grows no mark", !::grown(document, "bare.scroll"));
  status |= check(
    "the flag reads back",
    !GET::scrolling(document, "bare") && GET::scrolling(document, "deep"));
  remove(document);
  return status;
}
