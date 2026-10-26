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
#define GLFW_INCLUDE_VULKAN
#include <island/window/backend/glfw/window.internal.hpp>
#include <stdexcept>

GLFWwindow *WINDOW::BACKEND::GLFW::window = nullptr;
using WINDOW::BACKEND::GLFW::window;

Flag WINDOW::BACKEND::visible = true;
Flag WINDOW::BACKEND::decorated = true;

static void report(int error, STRING::Hot description) {
  LOGGER::get("~/island/window/glfw::errors")
    .debug("GLFW error %d: %s", error, description);
}

void WINDOW::BACKEND::initialize() {
  glfwSetErrorCallback(&report);
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
}

void WINDOW::BACKEND::create(STRING::Hot title, int width, int height) {
  if (width <= 0 || height <= 0) {
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = monitor ? glfwGetVideoMode(monitor) : nullptr;
    width = mode ? mode->width : FALLBACK::WIDTH;
    height = mode ? mode->height : FALLBACK::HEIGHT;
  }
  glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);
  glfwWindowHint(GLFW_DECORATED, decorated ? GLFW_TRUE : GLFW_FALSE);
  window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window) {
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwSetWindowPos(window, ORIGIN::X, ORIGIN::Y);
  if (visible) glfwShowWindow(window);
  GLFW::wire();
  GLFW::track();
}

Flag WINDOW::BACKEND::poll() {
  glfwPollEvents();
  GLFW::follow();
  return !glfwWindowShouldClose(window);
}

void WINDOW::BACKEND::GET::size(int *width, int *height) {
  glfwGetFramebufferSize(window, width, height);
}

Float WINDOW::BACKEND::GET::scale() {
  Float x = 1.0f, y = 1.0f;
  glfwGetWindowContentScale(window, &x, &y);
  return x;
}

void WINDOW::BACKEND::destroy() {
  GLFW::forget();
  if (window) {
    glfwDestroyWindow(window);
    window = nullptr;
  }
  glfwTerminate();
}

#if SR_GFX_BACKEND == SR_VULKAN
Vector<STRING::Hot> WINDOW::BACKEND::GET::extensions() {
  uint32_t count = 0;
  const char **extensions = glfwGetRequiredInstanceExtensions(&count);
  return Vector<STRING::Hot>(extensions, extensions + count);
}

VkSurfaceKHR WINDOW::BACKEND::SURFACE::create(VkInstance instance) {
  VkSurfaceKHR surface;
  if (
    glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
    VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
  return surface;
}
#endif

#endif
