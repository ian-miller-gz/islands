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

using WINDOW::BACKEND::GLFW::window;

namespace {
Flag holding = false;
double grabx = 0;
double graby = 0;

}  // namespace

void WINDOW::BACKEND::close() { glfwSetWindowShouldClose(window, GLFW_TRUE); }

void WINDOW::BACKEND::minimize() { glfwIconifyWindow(window); }

auto WINDOW::BACKEND::GET::maximized() -> Flag {
  return glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
}

void WINDOW::BACKEND::maximize(Flag on) {
  if (!on) return glfwRestoreWindow(window);
  GLFW::keep();
  glfwMaximizeWindow(window);
}

void WINDOW::BACKEND::drag(Flag on) {
  holding = on;
  if (on) glfwGetCursorPos(window, &grabx, &graby);
}

void WINDOW::BACKEND::GLFW::follow() {
  if (!holding) return;
  double x = 0, y = 0;
  glfwGetCursorPos(window, &x, &y);
  int left = 0, top = 0;
  glfwGetWindowPos(window, &left, &top);
  glfwSetWindowPos(
    window, left + static_cast<int>(x - grabx),
    top + static_cast<int>(y - graby));
}

#endif
