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
#include <iostream>

namespace TEXT = INPUT::TEXT;
namespace KEYS = INPUT::KEYS;

namespace WINDOW::BACKEND {
void capture(Flag) {}
void clipboard(STRING::Hot) {}
namespace GET {
String clipboard() { return {}; }
}  // namespace GET
}  // namespace WINDOW::BACKEND

static auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

static auto idle() -> Flag { return !TEXT::wanted() && TEXT::drain().empty(); }

static auto stream() -> Flag {
  TEXT::focus(true);
  if (!TEXT::wanted()) return false;
  TEXT::deliver({KEYS::TEXT, 'h'});
  TEXT::deliver({KEYS::TEXT, 'i'});
  TEXT::deliver({KEYS::LEFT});
  const auto events = TEXT::drain();
  const Flag routed = events.size() == 3 && events[0].codepoint == 'h' &&
                      events[1].codepoint == 'i' &&
                      events[2].action == KEYS::LEFT;
  return routed && TEXT::drain().empty();
}

static auto yielded() -> Flag {
  TEXT::focus(true);
  TEXT::deliver({KEYS::TEXT, 'x'});
  TEXT::claim(true);
  const Flag dropped = !TEXT::wanted() && TEXT::drain().empty();
  TEXT::claim(false);
  return dropped && !TEXT::wanted();
}

static auto released() -> Flag {
  TEXT::focus(true);
  TEXT::deliver({KEYS::TEXT, 'y'});
  TEXT::focus(false);
  return !TEXT::wanted() && TEXT::drain().empty();
}

int main() {
  Status status = 0;
  status |= check("idle", idle());
  status |= check("stream", stream());
  status |= check("yielded", yielded());
  status |= check("released", released());
  return status;
}
