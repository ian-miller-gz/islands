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
#include <island/input.hpp>
#include <island/window/backend.hpp>
#include <array>
#include <chrono>

static INPUT::Pointer state;
static INPUT::Pointer parked;

static Float atX = 0;
static Float atY = 0;
static Flag met = false;
static Float movedX = 0;
static Float movedY = 0;
static Float rolled = 0;
static Flag chorded = false;
static INPUT::Chord chord;
static std::array<Flag, 3> buttons{};
static INPUT::Edge rose;
static INPUT::Edge fell;
static Flag claimed = false;
static Flag captured = false;
static INPUT::Stamp folded = 0;

void INPUT::POINTER::move(Float x, Float y) {
  if (met) {
    movedX += x - atX;
    movedY += y - atY;
  }
  atX = x;
  atY = y;
  met = true;
}

void INPUT::POINTER::drag(Float dx, Float dy) {
  movedX += dx;
  movedY += dy;
}

void INPUT::POINTER::press(Button button, Flag down, Chord chord) {
  if (button == LEFT) {
    (down ? ::rose : ::fell) = {true, atX, atY};
    if (down) ::chord = chord;
  }
  buttons[button] = down;
}

void INPUT::POINTER::scroll(Float amount, Flag control) {
  rolled += amount;
  if (control) chorded = true;
}

void INPUT::POINTER::claim(Flag owned) {
  if (owned) capture(false);
  claimed = owned;
}

void INPUT::POINTER::capture(Flag on) {
  if (on == captured) return;
  if (on && claimed) return;
  captured = on;
  WINDOW::capture(on);
  met = false;
}

void INPUT::POINTER::frame() {
  if (!captured && met) {
    state.x = atX;
    state.y = atY;
  }
  state.dx = movedX;
  state.dy = movedY;
  state.wheel = rolled;
  state.control = chorded;
  state.left = buttons[LEFT];
  state.rose = rose;
  state.fell = fell;
  state.chord = chord;
  state.middle = buttons[MIDDLE];
  state.right = buttons[RIGHT];
  movedX = 0;
  movedY = 0;
  rolled = 0;
  chorded = false;
  if (fell.on) chord = {};
  rose = {};
  fell = {};
  folded = std::chrono::steady_clock::now().time_since_epoch().count();
}

auto INPUT::GET::stamped() -> INPUT::Stamp { return folded; }

auto INPUT::GET::pointer() -> const INPUT::Pointer& {
  if (!claimed) return state;
  parked = {};
  parked.x = state.x;
  parked.y = state.y;
  return parked;
}
