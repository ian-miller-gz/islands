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

namespace CONSOLES {

struct Console;

using Interpreter = void (*)(Console &console, const String &line);

struct Color {
  Float r, g, b, a;
};

namespace DISPATCH {
void interpret(Console &console, const String &line);

inline Whole hotkey = 0;
}  // namespace DISPATCH

constexpr Whole LIMIT = 1000;
constexpr Float MARGIN = 8;
constexpr Float SHARE = 0.5f;
constexpr Float SIZE = 15;
constexpr STRING::Hot PROMPT = "> ";
constexpr STRING::Hot CURSOR = "_";
constexpr Color COLOR = {0.26f, 0.30f, 0.36f, 1.0f};

struct Console {
  Vector<String> lines;
  String line;
  Flag visible = false;
  STRING::Hot prompt = PROMPT;
  Color color = COLOR;
  Float share = SHARE;
  Float margin = MARGIN;
  Whole scroll = 0;
  Whole rows = 0;
  Whole hotkey = 0;
  Flag guest = false;
  Interpreter shell = &DISPATCH::interpret;
  Interpreter home = &DISPATCH::interpret;
};

inline Flag visible = false;

void initialize();

void append(Console &console, const String &line);

void page(Console &console, Integer pages);

void submit(Console &console);

void close();

auto create() -> Console &;

void remove(Console &console);

void remove();

namespace GET {
auto consoles() -> Vector<Console> &;
auto console() -> Console &;
}  // namespace GET

}  // namespace CONSOLES
