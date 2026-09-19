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

auto sized(GUI::Handle document, STRING::Hot id, Float w, Float h) -> Flag {
  const auto extent = GUI::GET::measured(document, id);
  return extent.w == w && extent.h == h;
}

}  // namespace

Status resize() {
  using namespace GUI;
  Handle document = create("resize");
  Status status = check("create resize document", document != NONE);
  status |= NODES::create(document, NODES::ROOT, "panel", "fill");
  status |= set(document, "fill", Position{20.0f, 20.0f});
  status |= set(document, "fill", Extent{20.0f, 20.0f});
  status |= set(document, "fill", Stretch{EDGE::FILL});
  status |= NODES::create(document, "fill", "panel", "rail");
  status |= set(document, "rail", Extent{100.0f, 0.0f});
  status |= set(document, "rail", Stretch{EDGE::TOP | EDGE::BOTTOM});

  GFX::Pass pass = {.viewport = {0.0f, 0.0f, 400.0f, 300.0f}};
  flush(document, pass);
  status |= check("stretch fills", sized(document, "fill", 360.0f, 260.0f));
  status |=
    check("rail tracks height", sized(document, "rail", 100.0f, 260.0f));

  pass.viewport = {0.0f, 0.0f, 800.0f, 600.0f};
  flush(document, pass);
  status |= check("stretch re-fits", sized(document, "fill", 760.0f, 560.0f));
  status |= check("rail re-fits", sized(document, "rail", 100.0f, 560.0f));

  const auto authored = GET::extent(document, "fill");
  status |= check(
    "authored extent is the margin",
    authored.w == 20.0f && authored.h == 20.0f);
  remove(document);
  return status;
}
