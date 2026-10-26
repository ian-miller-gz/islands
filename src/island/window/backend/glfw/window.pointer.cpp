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
#include <island/input.hpp>
#include <island/window/backend/glfw/window.internal.hpp>

static void motion(GLFWwindow *, double x, double y) {
  INPUT::POINTER::move(static_cast<Float>(x), static_cast<Float>(y));
}

static void button(GLFWwindow *, int button, int action, int mods) {
  const Flag down = action == GLFW_PRESS;
  const INPUT::Chord chord = {
    (mods & GLFW_MOD_CONTROL) != 0, (mods & GLFW_MOD_SHIFT) != 0,
    (mods & GLFW_MOD_ALT) != 0};
  if (button == GLFW_MOUSE_BUTTON_LEFT)
    INPUT::POINTER::press(INPUT::POINTER::LEFT, down, chord);
  if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    INPUT::POINTER::press(INPUT::POINTER::MIDDLE, down, chord);
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    INPUT::POINTER::press(INPUT::POINTER::RIGHT, down, chord);
}

static void focus(GLFWwindow *, int focused) {
  if (focused == GLFW_TRUE) return;
  INPUT::POINTER::press(INPUT::POINTER::LEFT, false);
  INPUT::POINTER::press(INPUT::POINTER::MIDDLE, false);
  INPUT::POINTER::press(INPUT::POINTER::RIGHT, false);
}

static void wheel(GLFWwindow *held, double, double vertical) {
  const Flag control = glfwGetKey(held, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                       glfwGetKey(held, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
  INPUT::POINTER::scroll(static_cast<Float>(vertical), control);
}

void WINDOW::BACKEND::GLFW::track() {
  glfwSetCursorPosCallback(window, &motion);
  glfwSetMouseButtonCallback(window, &button);
  glfwSetScrollCallback(window, &wheel);
  glfwSetWindowFocusCallback(window, &focus);
}

void WINDOW::BACKEND::capture(Flag on) {
  glfwSetInputMode(
    GLFW::window, GLFW_CURSOR, on ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

#endif
