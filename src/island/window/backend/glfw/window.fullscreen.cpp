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
struct Placement {
  int x, y, width, height;
};
Placement windowed = {
  WINDOW::BACKEND::ORIGIN::X, WINDOW::BACKEND::ORIGIN::Y,
  WINDOW::BACKEND::FALLBACK::WIDTH, WINDOW::BACKEND::FALLBACK::HEIGHT};
}  // namespace

auto WINDOW::BACKEND::GET::live() -> Flag { return window != nullptr; }

auto WINDOW::BACKEND::GET::fullscreen() -> Flag {
  return glfwGetWindowMonitor(window) != nullptr;
}

void WINDOW::BACKEND::GLFW::keep() {
  if (GET::fullscreen() || GET::maximized()) return;
  glfwGetWindowPos(window, &windowed.x, &windowed.y);
  glfwGetWindowSize(window, &windowed.width, &windowed.height);
}

void WINDOW::BACKEND::fullscreen(Flag on) {
  if (on == GET::fullscreen()) return;
  if (!on)
    return glfwSetWindowMonitor(
      window, nullptr, windowed.x, windowed.y, windowed.width, windowed.height,
      0);
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = monitor ? glfwGetVideoMode(monitor) : nullptr;
  if (!mode) return;
  GLFW::keep();
  glfwSetWindowMonitor(
    window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

#endif
