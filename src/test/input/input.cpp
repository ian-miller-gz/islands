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
#include <iostream>
#include <island/audio/audio.internal.hpp>
#include "input.internal.hpp"

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

static auto silent() -> Flag {
  return AUDIO::INPUT::create(RATE, STEREO) == AUDIO::INPUT::NONE &&
         AUDIO::BACKEND::recorders() == 0;
}

static auto deferred() -> Flag {
  if (AUDIO::BACKEND::recorders() != 0) return false;
  const Flag refused = AUDIO::INPUT::create(0, STEREO) == AUDIO::INPUT::NONE &&
                       AUDIO::INPUT::create(RATE, 0) == AUDIO::INPUT::NONE;
  return refused && AUDIO::BACKEND::recorders() == 0;
}

static auto named() -> Flag {
  for (const auto &row : AUDIO::INPUT::GET::devices())
    if (row.name.empty()) return false;
  const auto before = AUDIO::BACKEND::recorders();
  const Flag refused =
    AUDIO::INPUT::create(RATE, STEREO, "elsewhere") == AUDIO::INPUT::NONE;
  const auto shared = AUDIO::INPUT::create(RATE, STEREO, "");
  const Flag held = AUDIO::BACKEND::recorders() == before;
  if (shared != AUDIO::INPUT::NONE) AUDIO::INPUT::remove(shared);
  return refused && shared != AUDIO::INPUT::NONE && held;
}

static auto several(Whole standing) -> Flag {
  const auto first = AUDIO::INPUT::create(RATE, STEREO, "");
  const auto second = AUDIO::INPUT::create(SLOW, MONO, "");
  if (first == AUDIO::INPUT::NONE || second == AUDIO::INPUT::NONE) return false;
  if (AUDIO::BACKEND::recorders() != standing) return false;
  AUDIO::INPUT::remove(first);
  const Flag kept = AUDIO::BACKEND::recorders() == standing;
  AUDIO::INPUT::remove(second);
  Flag beside = true;
  for (const auto &row : AUDIO::INPUT::GET::devices()) {
    const auto other = AUDIO::INPUT::create(RATE, STEREO, row.name);
    beside = other != AUDIO::INPUT::NONE &&
             AUDIO::BACKEND::recorders() == standing + 1;
    AUDIO::INPUT::remove(other);
    beside = beside && AUDIO::BACKEND::recorders() == standing;
    break;
  }
  return kept && beside;
}

int main() {
  Status status = check("silent", silent());
  COMMON::PLATFORM::assign("SDL_AUDIO_DRIVER", "dummy");
  AUDIO::initialize();
  status |= check("deferred", deferred());
  const Whole first = AUDIO::INPUT::create(RATE, STEREO);
  const Whole other = AUDIO::INPUT::create(SLOW, MONO);
  status |= check(
    "created", first != AUDIO::INPUT::NONE && other != AUDIO::INPUT::NONE &&
                 first != other && AUDIO::BACKEND::recorders() == 1);
  status |= check("named", named());
  status |= check("several", several(1));
  status |= check("arriving", arriving(first, other));
  status |= check("drained", drained(first));
  status |= check("stopped", stopped(first));
  status |= check("missing", missing());
  status |= check("driven", driven());
  status |= check("removed", removed(first, other));
  return status;
}
