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
#include <island/graphics/windows.hpp>
#include <cmath>
#include <iostream>

constexpr Float EPSILON = 0.001f;

auto VIEW::check(const String &name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

void VIEW::window(int width, int height) {
  GFX::WINDOWS::MAIN::width = width;
  GFX::WINDOWS::MAIN::height = height;
}

auto VIEW::matches(Float value, Float expected) -> Flag {
  return std::fabs(value - expected) < EPSILON;
}

auto VIEW::fits(const GFX::Viewport &rect) -> Flag {
  const auto wide = static_cast<Float>(GFX::WINDOWS::MAIN::width);
  const auto tall = static_cast<Float>(GFX::WINDOWS::MAIN::height);
  return rect.x >= 0.0f && rect.y >= 0.0f && rect.x + rect.w <= wide + 1.0f &&
         rect.y + rect.h <= tall + 1.0f;
}

auto main() -> int {
  Status status = 0;
  status |= VIEW::integer();
  status |= VIEW::letterbox();

  GFX::VIEW::create(VIEW::WIDE, VIEW::TALL);
  VIEW::window(1200, 700);
  status |= VIEW::check(
    "default fit steps (scale 3)",
    VIEW::matches(GFX::VIEW::GET::scale(), 3.0f));
  GFX::VIEW::reset();
  return status;
}
