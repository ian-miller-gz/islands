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
#include <island/window/backend.hpp>
#include <iostream>

namespace {

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

constexpr Whole SPAN = 8;
constexpr int EXTENT = 64;

auto square(Byte tint) -> WINDOW::Image {
  WINDOW::Image image{SPAN, SPAN, {}};
  image.pixels.assign(SPAN * SPAN * WINDOW::CHANNELS, tint);
  for (Whole pixel = 0; pixel < SPAN * SPAN; ++pixel)
    image.pixels[pixel * WINDOW::CHANNELS + 3] = static_cast<Byte>(0xFF);
  return image;
}

auto walked() -> Status {
  Status status =
    check("a fresh window wears no picture", !WINDOW::GET::worn());
  WINDOW::wear(square(0x40), SPAN / 2, SPAN / 2);
  status |= check("a picture goes on", WINDOW::GET::worn());
  WINDOW::wear(square(0x80), 0, 0);
  status |= check("a second picture replaces it", WINDOW::GET::worn());
  WINDOW::shape(WINDOW::IBEAM);
  status |= check("a standard shape takes it off", !WINDOW::GET::worn());
  WINDOW::wear(square(0x40), SPAN * SPAN, SPAN * SPAN);
  status |= check("a hotspot past the edge clamps inside", WINDOW::GET::worn());
  WINDOW::shape(WINDOW::ARROW);
  status |= check("the arrow takes it off too", !WINDOW::GET::worn());
  WINDOW::wear({}, 0, 0);
  status |= check("an empty image is refused", !WINDOW::GET::worn());
  const WINDOW::Image scant{SPAN, SPAN, Vector<Byte>(SPAN * SPAN, 0)};
  WINDOW::wear(scant, 0, 0);
  return status |= check(
           "an image short of its own bytes is refused", !WINDOW::GET::worn());
}

}  // namespace

int main() {
  WINDOW::visible = false;
  WINDOW::initialize();
  WINDOW::create("worn", EXTENT, EXTENT);
  Status status = walked();
  WINDOW::wear(square(0x40), 0, 0);
  WINDOW::destroy();
  status |= check("the teardown drops the picture", !WINDOW::GET::worn());
  return status;
}
