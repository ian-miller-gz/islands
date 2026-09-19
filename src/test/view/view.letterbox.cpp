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
#include <string>

constexpr int SIZES[][2] = {{1280, 720}, {1200, 700}, {641, 361},
                            {200, 400},  {400, 100},  {64, 36}};

auto VIEW::letterbox() -> Status {
  Status status = 0;
  GFX::VIEW::create(WIDE, TALL, GFX::VIEW::COMPOSED, GFX::VIEW::LETTERBOX);
  status |=
    check("letterbox presents bilinear", sampled() == GFX::SAMPLERS::LINEAR);

  window(1280, 720);
  auto rect = GFX::VIEW::GET::viewport();
  status |=
    check("letterbox 1280x720 scale 4", matches(GFX::VIEW::GET::scale(), 4.0f));
  status |= check(
    "letterbox 1280x720 fills the window",
    matches(rect.w, 1280.0f) && matches(rect.h, 720.0f));

  window(1200, 700);
  rect = GFX::VIEW::GET::viewport();
  status |= check(
    "letterbox 1200x700 scale 3.75", matches(GFX::VIEW::GET::scale(), 3.75f));
  status |= check(
    "letterbox 1200x700 spans the width",
    matches(rect.x, 0.0f) && matches(rect.w, 1200.0f) &&
      matches(rect.y, 12.0f) && matches(rect.h, 675.0f));

  window(200, 100);
  rect = GFX::VIEW::GET::viewport();
  status |= check(
    "letterbox 200x100 scales below 1",
    matches(GFX::VIEW::GET::scale(), 100.0f / static_cast<Float>(TALL)));
  status |= check("letterbox 200x100 keeps the view whole", fits(rect));

  for (const auto &size : SIZES) {
    window(size[0], size[1]);
    const String extent =
      std::to_string(size[0]) + "x" + std::to_string(size[1]);
    status |=
      check("letterbox " + extent + " whole", fits(GFX::VIEW::GET::viewport()));
  }
  GFX::VIEW::reset();
  return status;
}
