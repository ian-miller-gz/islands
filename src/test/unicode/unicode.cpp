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
#include <common.hpp>
#include <iostream>

constexpr Whole ASCII = 0x80;
constexpr Whole ACUTE = 0xE9;
constexpr Whole DASH = 0x2014;
constexpr Whole GRINNING = 0x1F600;

static auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

static auto reads(const String &text, Whole want, Whole bytes) -> Flag {
  Whole index = 0;
  const auto code = UNICODE::decode(text, index);
  return code == want && index == bytes;
}

static auto walks(const String &text, const Vector<Whole> &want) -> Flag {
  Vector<Whole> codes;
  const auto size = static_cast<Whole>(text.size());
  for (Whole index = 0; index < size;)
    codes.push_back(UNICODE::decode(text, index));
  return codes == want;
}

static auto ascii() -> Flag {
  for (Whole code = 0; code < ASCII; code += 1)
    if (!reads(String(1, static_cast<Char>(code)), code, 1)) return false;
  return true;
}

static auto round(Whole code) -> Flag {
  Whole index = 0;
  return UNICODE::decode(UNICODE::encode(code), index) == code && index > 0;
}

static auto backwards() -> Flag {
  const String run = "a\xe2\x80\x94\x62";
  Whole index = 5;
  UNICODE::rewind(run, index);
  if (index != 4) return false;
  UNICODE::rewind(run, index);
  if (index != 1) return false;
  UNICODE::rewind(run, index);
  return index == 0;
}

int main() {
  Status status = 0;
  status |= check("ascii", ascii());
  status |= check(
    "multibyte", reads("\xc3\xa9", ACUTE, 2) &&
                   reads("\xe2\x80\x94", DASH, 3) &&
                   reads("\xf0\x9f\x98\x80", GRINNING, 4));
  status |= check(
    "stray", reads("\x80", UNICODE::REPLACEMENT, 1) &&
               reads("\xf8", UNICODE::REPLACEMENT, 1));
  status |= check("truncated", reads("\xe2\x80", UNICODE::REPLACEMENT, 2));
  status |= check("broken", walks("\xe2Z", {UNICODE::REPLACEMENT, 'Z'}));
  status |= check("overlong", reads("\xc0\xaf", UNICODE::REPLACEMENT, 2));
  status |= check("surrogate", reads("\xed\xa0\x80", UNICODE::REPLACEMENT, 3));
  status |= check("beyond", reads("\xf5\x80\x80\x80", UNICODE::REPLACEMENT, 4));
  status |= check("run", walks("a\xe2\x80\x94\x62", {'a', DASH, 'b'}));
  status |= check(
    "control", UNICODE::control(0) && UNICODE::control('\n') &&
                 UNICODE::control(0x7F) && UNICODE::control(0x9F) &&
                 !UNICODE::control(' ') && !UNICODE::control(DASH));
  status |= check(
    "encode",
    round('A') && round(ACUTE) && round(DASH) && round(GRINNING) &&
      UNICODE::encode(0xD800) == UNICODE::encode(UNICODE::REPLACEMENT) &&
      UNICODE::encode(0x110000) == UNICODE::encode(UNICODE::REPLACEMENT));
  status |= check("rewind", backwards());
  return status;
}
