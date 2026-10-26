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
#include <island/input.hpp>

static void character(GLFWwindow *, unsigned int codepoint) {
  INPUT::KEYS::push({INPUT::KEYS::TEXT, codepoint});
}

static auto ascii(int key) -> Whole {
  if (key >= GLFW_KEY_SPACE && key <= GLFW_KEY_GRAVE_ACCENT)
    return static_cast<Whole>(key);
  if (key == GLFW_KEY_TAB) return INPUT::KEYS::TAB;
  if (key == GLFW_KEY_ESCAPE) return INPUT::KEYS::ESCAPE;
  return 0;
}

static void navigate(int key, int mods) {
  using namespace INPUT::KEYS;
  const auto move = [mods](Action action) {
    push(
      {action, 0, (mods & GLFW_MOD_CONTROL) != 0, (mods & GLFW_MOD_SHIFT) != 0,
       (mods & GLFW_MOD_ALT) != 0});
  };
  if (key == GLFW_KEY_LEFT) move(LEFT);
  if (key == GLFW_KEY_RIGHT) move(RIGHT);
  if (key == GLFW_KEY_UP) move(UP);
  if (key == GLFW_KEY_DOWN) move(DOWN);
  if (key == GLFW_KEY_HOME) move(HOME);
  if (key == GLFW_KEY_END) move(END);
  if (key == GLFW_KEY_DELETE) move(DELETE);
  if (key == GLFW_KEY_PAGE_UP) move(PRIOR);
  if (key == GLFW_KEY_PAGE_DOWN) move(NEXT);
}

static void special(int key, int mods) {
  using namespace INPUT::KEYS;
  if (key == GLFW_KEY_TAB) push({TEXT, TAB});
  if (key == GLFW_KEY_ESCAPE) push({TEXT, ESCAPE});
  if ((mods & GLFW_MOD_CONTROL) && key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
    push({TEXT, static_cast<Whole>(key), true, (mods & GLFW_MOD_SHIFT) != 0});
}

static void key(GLFWwindow *, int key, int, int action, int mods) {
  if (action != GLFW_REPEAT) {
    if (const auto code = ascii(key))
      INPUT::KEYS::press(code, action == GLFW_PRESS);
    if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F12)
      INPUT::KEYS::press(
        INPUT::KEYS::FUNCTION + static_cast<Whole>(key - GLFW_KEY_F1 + 1),
        action == GLFW_PRESS);
  }
  if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
  if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER)
    INPUT::KEYS::push({INPUT::KEYS::ENTER});
  if (key == GLFW_KEY_BACKSPACE) INPUT::KEYS::push({INPUT::KEYS::ERASE});
  navigate(key, mods);
  special(key, mods);
  if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F12)
    INPUT::KEYS::push(
      {INPUT::KEYS::HOTKEY, static_cast<Whole>(key - GLFW_KEY_F1 + 1)});
}

void WINDOW::BACKEND::GLFW::wire() {
  glfwSetCharCallback(window, &character);
  glfwSetKeyCallback(window, &key);
}

#endif
