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
#include <island/gui/window.hpp>
#include <test/gui/gui.internal.hpp>

namespace {
constexpr Float WIDE = 200.0f;
constexpr Float DEEP = 100.0f;
constexpr Float SCALE = 2.0f;
}  // namespace

Status windowed() {
  using namespace GUI::SAC;
  const GUI::Extent face = {::WIDE, ::DEEP};
  Status status = check(
    "undesignated has no window",
    WINDOW::seen(face, {::SCALE}) == GUI::Extent{});
  status |= check("the name adopts", adopt({String(WINDOW::NAME)}) == 0);
  const GUI::Extent span = WINDOW::seen(face, {::SCALE});
  status |= check(
    "the span is the face over the scale", span.w == 100.0f && span.h == 50.0f);
  status |= check(
    "a SAME down scale takes the across value",
    WINDOW::seen(face, {::SCALE, GUI::NGA::Zoom::SAME}).h == 50.0f);
  status |= check(
    "a stated down scale is its own axis",
    WINDOW::seen(face, {::SCALE, 4.0f}).h == 25.0f);
  status |= check(
    "a board at no scale has no window",
    WINDOW::seen(face, {0.0f}) == GUI::Extent{});
  status |= check(
    "an unmeasured board has no window",
    WINDOW::seen({}, {::SCALE}) == GUI::Extent{});
  status |= check(
    "the floor fits the span whole", WINDOW::least(::WIDE, 50.0f) == 4.0f);
  status |= check(
    "a span of nothing has no floor", WINDOW::least(::WIDE, 0.0f) == 0.0f);
  status |= check(
    "an unmeasured axis has no floor", WINDOW::least(0.0f, 50.0f) == 0.0f);
  status |= check(
    "the wheel keeps the point under the pointer",
    WINDOW::about(10.0f, 30.0f, 1.0f, 2.0f) == 20.0f);
  status |= check(
    "and a scale of nothing moves no pan",
    WINDOW::about(10.0f, 30.0f, 1.0f, 0.0f) == 10.0f);
  status |= check(
    "a marker behind the middle is not chased",
    WINDOW::chase(0.0f, 40.0f, 100.0f) == 0.0f);
  status |= check(
    "and one at the middle takes the pan with it",
    WINDOW::chase(0.0f, 60.0f, 100.0f) == 10.0f);
  status |= check(
    "a thing in the window is left alone",
    WINDOW::centre(0.0f, 10.0f, 20.0f, 100.0f) == 0.0f);
  status |= check(
    "and one outside it lands in the middle",
    WINDOW::centre(0.0f, 200.0f, 20.0f, 100.0f) == 160.0f);
  const GUI::Walls walls =
    WINDOW::walled({20.0f, 60.0f, 20.0f, 40.0f}, {5.0f, 5.0f}, span);
  status |= check(
    "the walls fold the origin in and wander out",
    walls.west == -5.0f && walls.north == -5.0f);
  status |= check(
    "and each far wall is floored at the span",
    walls.east == 95.0f && walls.south == 45.0f);
  status |= check("re-adoption replaces whole", adopt({}) == 0);
  status |= check(
    "the doors closed again", WINDOW::seen(face, {::SCALE}) == GUI::Extent{});
  status |= check(
    "and the pans stand where they were",
    WINDOW::chase(7.0f, 60.0f, 100.0f) == 7.0f);
  return status;
}
