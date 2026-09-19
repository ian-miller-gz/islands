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
#include <test/view/view.internal.hpp>
#include <island/graphics/view/view.hpp>

auto VIEW::integer() -> Status {
  Status status = 0;
  GFX::VIEW::create(WIDE, TALL, GFX::VIEW::COMPOSED, GFX::VIEW::INTEGER);
  status |=
    check("integer presents nearest", sampled() == GFX::SAMPLERS::CLAMP);

  window(1280, 720);
  auto rect = GFX::VIEW::GET::viewport();
  status |=
    check("integer 1280x720 scale 4", matches(GFX::VIEW::GET::scale(), 4.0f));
  status |= check(
    "integer 1280x720 fills the window",
    matches(rect.x, 0.0f) && matches(rect.y, 0.0f) &&
      matches(rect.w, 1280.0f) && matches(rect.h, 720.0f));

  window(1200, 700);
  rect = GFX::VIEW::GET::viewport();
  status |=
    check("integer 1200x700 scale 3", matches(GFX::VIEW::GET::scale(), 3.0f));
  status |= check(
    "integer 1200x700 letterboxes 960x540",
    matches(rect.x, 120.0f) && matches(rect.y, 80.0f) &&
      matches(rect.w, 960.0f) && matches(rect.h, 540.0f));
  status |= check("integer 1200x700 stays inside", fits(rect));

  window(200, 100);
  rect = GFX::VIEW::GET::viewport();
  status |= check(
    "integer 200x100 clamps at 1", matches(GFX::VIEW::GET::scale(), 1.0f));
  status |= check(
    "integer 200x100 crops centered",
    matches(rect.x, -60.0f) && matches(rect.y, -40.0f) &&
      matches(rect.w, 320.0f) && matches(rect.h, 180.0f));
  status |= check("integer 200x100 clips the view", !fits(rect));

  GFX::VIEW::reset();
  return status;
}
