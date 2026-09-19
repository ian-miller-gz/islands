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
#include <island.hpp>
#include <island/window/backend.hpp>

#if SR_WINDOW_BACKEND == SR_GLFW
#include <island/window/backend/glfw/window.internal.hpp>
#include <iterator>

namespace {

constexpr int STANDARDS[] = {GLFW_ARROW_CURSOR, GLFW_IBEAM_CURSOR};
GLFWcursor *cursors[std::size(STANDARDS)] = {};
WINDOW::BACKEND::Shape standing = WINDOW::BACKEND::ARROW;
GLFWcursor *picture = nullptr;

void drop() {
  if (picture) glfwDestroyCursor(picture);
  picture = nullptr;
}

auto inside(Whole where, Whole span) -> int {
  return static_cast<int>(where < span ? where : span - 1);
}

}  // namespace

void WINDOW::BACKEND::shape(Shape wanted) {
  if (!GLFW::window || (wanted == standing && !picture)) return;
  if (!cursors[wanted])
    cursors[wanted] = glfwCreateStandardCursor(STANDARDS[wanted]);
  if (!cursors[wanted]) return;
  glfwSetCursor(GLFW::window, cursors[wanted]);
  drop();
  standing = wanted;
}

void WINDOW::BACKEND::wear(const Image &image, Whole hotx, Whole hoty) {
  if (!GLFW::window || !image.w || !image.h) return;
  if (image.pixels.size() < image.w * image.h * CHANNELS) return;
  GLFWimage source{
    .width = static_cast<int>(image.w),
    .height = static_cast<int>(image.h),
    .pixels = reinterpret_cast<unsigned char *>(
      const_cast<Byte *>(image.pixels.data()))};
  GLFWcursor *made =
    glfwCreateCursor(&source, inside(hotx, image.w), inside(hoty, image.h));
  if (!made) return;
  drop();
  picture = made;
  glfwSetCursor(GLFW::window, picture);
}

auto WINDOW::BACKEND::GET::worn() -> Flag { return picture != nullptr; }

void WINDOW::BACKEND::GLFW::forget() {
  for (auto *&cursor : cursors) {
    if (cursor) glfwDestroyCursor(cursor);
    cursor = nullptr;
  }
  drop();
  standing = ARROW;
}

#endif
