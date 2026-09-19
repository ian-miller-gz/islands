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
#include <island/gui/events.internal.hpp>

namespace {

constexpr STRING::Hot MARK = "…";
constexpr Whole SPENT = 1;

constexpr unsigned char CARRY = 0xC0u;
constexpr unsigned char CONTINUATION = 0x80u;

auto opens(char byte) -> Flag {
  return (static_cast<unsigned char>(byte) & CARRY) != CONTINUATION;
}

auto offset(const String &text, Whole wanted) -> String::size_type {
  Whole seen = 0;
  for (String::size_type at = 0; at < text.size(); ++at) {
    if (!opens(text[at])) continue;
    if (seen == wanted) return at;
    ++seen;
  }
  return text.size();
}

}  // namespace

auto GUI::fit(Float near, Float span, Float low, Float high) -> Float {
  const Float pushed = near + span > high ? high - span : near;
  return pushed < low ? low : pushed;
}

auto GUI::budget(Float width, Float step) -> Whole {
  if (width <= 0.0f || step <= 0.0f) return 0;
  return static_cast<Whole>(width / step);
}

auto GUI::cells(const String &text) -> Whole {
  Whole seen = 0;
  for (const char byte : text)
    if (opens(byte)) ++seen;
  return seen;
}

auto GUI::cut(const String &text, Whole budget) -> String {
  if (budget == 0) return text;
  if (cells(text) <= budget) return text;
  return text.substr(0, offset(text, budget - SPENT)) + MARK;
}
