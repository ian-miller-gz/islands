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
#include "clips.internal.hpp"

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

static auto block(Whole channels) -> Vector<AUDIO::Sample> {
  return Vector<AUDIO::Sample>(BLOCK * channels);
}

static AUDIO::Handle first = AUDIO::NONE;
static AUDIO::Handle second = AUDIO::NONE;

static auto queued() -> Flag {
  first = AUDIO::play(block(STEREO), RATE, STEREO);
  second = AUDIO::play(block(MONO), RATE, MONO, true);
  const Flag waiting = first != AUDIO::NONE && second != AUDIO::NONE &&
                       first != second && AUDIO::BACKEND::pending() == 2 &&
                       AUDIO::BACKEND::live() == 0;
  const Flag refused = AUDIO::play(block(STEREO), 0, STEREO) == AUDIO::NONE &&
                       AUDIO::play("nowhere.wav") == AUDIO::NONE;
  return waiting && refused && AUDIO::BACKEND::pending() == 2;
}

static auto fired() -> Flag {
  return AUDIO::BACKEND::pending() == 0 && AUDIO::BACKEND::live() == 2 &&
         first < second && AUDIO::BACKEND::backlog(first) > 0 &&
         AUDIO::BACKEND::backlog(second) > 0;
}

static auto buffered() -> Flag {
  const AUDIO::Handle clip = AUDIO::play(block(STEREO), RATE, STEREO);
  return clip != AUDIO::NONE && AUDIO::BACKEND::backlog(clip) > 0;
}

static auto refused() -> Flag {
  const Whole before = AUDIO::BACKEND::live();
  const Flag none =
    AUDIO::play(block(STEREO), 0, STEREO) == AUDIO::NONE &&
    AUDIO::play(block(STEREO), RATE, 0) == AUDIO::NONE &&
    AUDIO::play(Vector<AUDIO::Sample>(), RATE, MONO) == AUDIO::NONE;
  return none && AUDIO::BACKEND::live() == before;
}

static auto missing() -> Flag {
  const Whole before = AUDIO::BACKEND::live();
  return AUDIO::play("nowhere.wav") == AUDIO::NONE &&
         AUDIO::BACKEND::live() == before;
}

static auto handled() -> Flag {
  AUDIO::stop();
  const AUDIO::Handle clip = AUDIO::play(block(STEREO), RATE, STEREO);
  const Whole live = AUDIO::BACKEND::live();
  AUDIO::volume(clip, 0.5f);
  AUDIO::stop(clip);
  const Flag gone =
    AUDIO::BACKEND::live() == live - 1 && AUDIO::BACKEND::backlog(clip) < 0;
  AUDIO::stop(clip);
  AUDIO::volume(clip, 1.0f);
  return gone;
}

static auto looped() -> Flag {
  AUDIO::stop();
  const AUDIO::Handle once = AUDIO::play(block(STEREO), RATE, STEREO, false);
  const AUDIO::Handle forever = AUDIO::play(block(STEREO), RATE, STEREO, true);
  const Flag playing = once != AUDIO::NONE && forever != AUDIO::NONE &&
                       AUDIO::BACKEND::live() == 2;
  AUDIO::stop();
  return playing && AUDIO::BACKEND::live() == 0;
}

int main() {
  Status status = check("queued", queued());
  COMMON::PLATFORM::assign("SDL_AUDIO_DRIVER", "dummy");
  AUDIO::initialize();
  status |= check("fired", fired());
  status |= check("buffered", buffered());
  status |= check("refused", refused());
  AUDIO::Handle clip = AUDIO::NONE;
  status |= check("honored", honored(clip));
  status |= check("missing", missing());
  status |= check("handled", handled());
  status |= check("looped", looped());
  return status;
}
