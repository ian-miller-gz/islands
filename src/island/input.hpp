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
#pragma once

#include <common.hpp>
#include <island/input.bindings.hpp>

namespace INPUT::KEYS {

enum Action {
  TEXT,
  ENTER,
  ERASE,
  LEFT,
  RIGHT,
  UP,
  DOWN,
  HOME,
  END,
  DELETE,
  PRIOR,
  NEXT,
  HOTKEY
};

struct Event {
  Action action = TEXT;
  Whole codepoint = 0;
  Flag control = false;
  Flag shift = false;
  Flag alt = false;
};

void push(const Event &event);

auto drain() -> Vector<Event>;

constexpr Whole TAB = '\t';
constexpr Whole ESCAPE = 27;

constexpr Whole FUNCTION = 128;

void press(Whole codepoint, Flag down);

void claim(Flag owned);

void frame();
}  // namespace INPUT::KEYS

namespace INPUT {
struct Edge {
  Flag on = false;
  Float x = 0;
  Float y = 0;
};

struct Chord {
  Flag control = false;
  Flag shift = false;
  Flag alt = false;
};

struct Pointer {
  Float x = 0;
  Float y = 0;
  Float dx = 0;
  Float dy = 0;
  Float wheel = 0;
  Flag control = false;
  Flag left = false;
  Edge rose;
  Edge fell;
  Chord chord;
  Flag middle = false;
  Flag right = false;
};

using Stamp = int64_t;
}  // namespace INPUT

namespace INPUT::POINTER {
enum Button { LEFT, MIDDLE, RIGHT };

void move(Float x, Float y);

void drag(Float dx, Float dy);

void press(Button button, Flag down, Chord chord = {});

void scroll(Float amount, Flag control);

void claim(Flag owned);

void capture(Flag on);

void frame();
}  // namespace INPUT::POINTER

namespace INPUT::TEXT {
void focus(Flag on);

auto drain() -> Vector<KEYS::Event>;

auto wanted() -> Flag;

void deliver(const KEYS::Event &event);

void claim(Flag owned);
}  // namespace INPUT::TEXT

namespace INPUT::CLIPBOARD {
void write(const String &text);

auto read() -> String;
}  // namespace INPUT::CLIPBOARD

namespace INPUT::GET {
auto pointer() -> const INPUT::Pointer &;

auto stamped() -> INPUT::Stamp;
auto held(Whole codepoint) -> Flag;
auto pressed(Whole codepoint) -> Flag;
}  // namespace INPUT::GET

namespace INPUT {
inline void process() {
  KEYS::frame();
  POINTER::frame();
}
}  // namespace INPUT
