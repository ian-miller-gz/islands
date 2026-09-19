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
#include <shell/os/keys.hpp>
#include <iostream>

namespace KEYS = INPUT::KEYS;
namespace ENCODE = SHELL::OS::KEYS;

static auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

static auto is(const KEYS::Event &event, const String &want) -> Flag {
  return ENCODE::encode(event) == want;
}

int main() {
  using namespace INPUT::KEYS;
  Status status = 0;
  status |= check(
    "arrows", is({LEFT}, "\x1b[D") && is({RIGHT}, "\x1b[C") &&
                is({UP}, "\x1b[A") && is({DOWN}, "\x1b[B"));
  status |= check("home/end", is({HOME}, "\x1b[H") && is({END}, "\x1b[F"));
  status |= check("delete", is({DELETE}, "\x1b[3~"));
  status |= check("paging", is({PRIOR}, "\x1b[5~") && is({NEXT}, "\x1b[6~"));
  status |= check("enter", is({ENTER}, "\r"));
  status |= check("erase", is({ERASE}, "\x7f"));
  status |= check("text", is({TEXT, 'a'}, "a") && is({TEXT, '~'}, "~"));
  status |=
    check("tab/escape", is({TEXT, TAB}, "\t") && is({TEXT, ESCAPE}, "\x1b"));
  status |= check(
    "control",
    is({TEXT, 'C', true}, String(1, 3)) && is({TEXT, 'D', true}, String(1, 4)));
  status |= check(
    "cursors", ENCODE::encode({UP}, true) == "\x1bOA" &&
                 ENCODE::encode({HOME}, true) == "\x1bOH" &&
                 ENCODE::encode({ENTER}, true) == "\r" &&
                 ENCODE::encode({TEXT, 'a'}, true) == "a");
  status |= check(
    "empty", ENCODE::encode({HOTKEY, 10}).empty() &&
               ENCODE::encode({TEXT, 0x100}).empty());
  return status;
}
