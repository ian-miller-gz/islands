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
#include <common/unicode.hpp>

constexpr Whole SHIFT = 6;
constexpr Whole PAYLOAD = 0x3F;
constexpr Whole MARK = 0xC0;
constexpr Whole SIGN = 0x80;
constexpr Whole PLANES = 0x10FFFF;
constexpr Whole SURROGATES = 0xD800, RESUME = 0xE000;
constexpr Whole PRINTABLE = 0x20;
constexpr Whole DELETE = 0x7F;
constexpr Whole CONTROLS = 0x9F;
constexpr Whole LEAD2 = 0xC0, LEAD3 = 0xE0, LEAD4 = 0xF0;

struct Shape {
  Whole ceiling = 0;
  Whole bits = 0;
  Whole length = 0;
  Whole floor = 0;
};

constexpr Shape SHAPES[] = {
  {.ceiling = 0x80, .bits = 0x7F, .length = 1},
  {.ceiling = 0xC0},
  {.ceiling = 0xE0, .bits = 0x1F, .length = 2, .floor = 0x80},
  {.ceiling = 0xF0, .bits = 0x0F, .length = 3, .floor = 0x800},
  {.ceiling = 0xF8, .bits = 0x07, .length = 4, .floor = 0x10000}};

static auto shape(Whole lead) -> Shape {
  for (const auto &entry : SHAPES)
    if (lead < entry.ceiling) return entry;
  return {};
}

static auto byte(const String &text, Whole index) -> Whole {
  return static_cast<Whole>(static_cast<unsigned char>(text[index]));
}

static auto ranged(Whole code, Whole floor) -> Flag {
  if (code < floor || code > PLANES) return false;
  return code < SURROGATES || code >= RESUME;
}

auto UNICODE::decode(const String &text, Whole &index) -> Whole {
  const auto size = static_cast<Whole>(text.size());
  if (index >= size) {
    index += 1;
    return REPLACEMENT;
  }
  const auto lead = ::byte(text, index);
  const auto found = ::shape(lead);
  index += 1;
  if (!found.length) return REPLACEMENT;
  Whole code = lead & found.bits;
  for (Whole step = 1; step < found.length; step += 1) {
    if (index >= size) return REPLACEMENT;
    const auto next = ::byte(text, index);
    if ((next & MARK) != SIGN) return REPLACEMENT;
    code = (code << SHIFT) | (next & PAYLOAD);
    index += 1;
  }
  return ::ranged(code, found.floor) ? code : REPLACEMENT;
}

constexpr Whole PREFIX[] = {0, 0, LEAD2, LEAD3, LEAD4};

static auto span(Whole code) -> Whole {
  Whole length = 1;
  for (const auto &entry : SHAPES)
    if (entry.length && code >= entry.floor && entry.length > length)
      length = entry.length;
  return length;
}

auto UNICODE::encode(Whole code) -> String {
  if (!::ranged(code, 0)) code = REPLACEMENT;
  const auto length = ::span(code);
  String out(length, '\0');
  for (Whole step = length; step > 1; step -= 1) {
    out[step - 1] = static_cast<Char>(SIGN | (code & PAYLOAD));
    code >>= SHIFT;
  }
  out[0] = static_cast<Char>(PREFIX[length] | code);
  return out;
}

void UNICODE::rewind(const String &text, Whole &index) {
  if (index == 0) return;
  index -= 1;
  while (index > 0 && (::byte(text, index) & MARK) == SIGN) index -= 1;
}

auto UNICODE::columns(const String &text) -> Whole {
  Whole count = 0;
  for (Whole index = 0; index < text.size(); count += 1) decode(text, index);
  return count;
}

auto UNICODE::control(Whole code) -> Flag {
  return code < PRINTABLE || (code >= DELETE && code <= CONTROLS);
}
