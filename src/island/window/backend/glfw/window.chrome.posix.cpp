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

extern "C" unsigned long glfwGetX11Window(GLFWwindow *);

auto WINDOW::BACKEND::GET::parent() -> Native {
  if (glfwGetPlatform() != GLFW_PLATFORM_X11) return NONE;
  return glfwGetX11Window(window);
}

#endif
