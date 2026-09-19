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
#include <cstdlib>
#include <SDL3/SDL_hints.h>
#include <iostream>
#include "output.internal.hpp"

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

static auto silent() -> Flag {
  return AUDIO::OUTPUT::create(RATE, STEREO) == AUDIO::OUTPUT::NONE;
}

static auto refused() -> Flag {
  return AUDIO::OUTPUT::create(0, STEREO) == AUDIO::OUTPUT::NONE &&
         AUDIO::OUTPUT::create(RATE, 0) == AUDIO::OUTPUT::NONE;
}

auto named() -> Flag {
  const Vector<AUDIO::OUTPUT::Device> roster = AUDIO::OUTPUT::GET::devices();
  const Whole fallback = AUDIO::OUTPUT::create(RATE, STEREO, "");
  Flag ok =
    fallback != AUDIO::OUTPUT::NONE &&
    AUDIO::OUTPUT::create(RATE, STEREO, "nowhere") == AUDIO::OUTPUT::NONE;
  AUDIO::OUTPUT::remove(fallback);
  if (roster.empty()) return ok;
  const Whole fed = AUDIO::OUTPUT::create(RATE, STEREO, roster[0].name);
  ok = ok && fed != AUDIO::OUTPUT::NONE && !roster[0].name.empty();
  AUDIO::OUTPUT::remove(fed);
  return ok;
}

auto preferred() -> Flag {
  const Vector<AUDIO::OUTPUT::Device> roster = AUDIO::OUTPUT::GET::devices();
  const String name = AUDIO::OUTPUT::GET::preferred();
  if (roster.empty()) return true;
  for (const AUDIO::OUTPUT::Device &device : roster)
    if (device.name == name) return !name.empty();
  return false;
}

auto monitored() -> Flag {
  const char *hint = SDL_GetHint(SDL_HINT_AUDIO_INCLUDE_MONITORS);
  return hint != nullptr && String(hint) == "1";
}

int main() {
  Status status = check("silent", silent());
  COMMON::PLATFORM::assign("SDL_AUDIO_DRIVER", "dummy");
  AUDIO::initialize();
  const Whole first = AUDIO::OUTPUT::create(RATE, STEREO);
  const Whole other = AUDIO::OUTPUT::create(SLOW, MONO);
  status |= check(
    "created", first != AUDIO::OUTPUT::NONE && other != AUDIO::OUTPUT::NONE &&
                 first != other);
  status |= check("refused", refused());
  status |= check("fed", fed(first));
  status |= check("mixed", mixed(first, other));
  status |= check("stopped", stopped(first));
  status |= check("missing", missing());
  status |= check("pulled", pulled());
  status |= check("named", named());
  status |= check("preferred", preferred());
  status |= check("monitored", monitored());
  status |= check("removed", removed(first, other));
  return status;
}
