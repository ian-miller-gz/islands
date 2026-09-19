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
#include <SDL3/SDL.h>
#include <island/audio/audio.internal.hpp>
#include "input.internal.hpp"

static auto settled(Whole stream) -> Flag {
  for (Whole waited = 0; waited < SETTLE; waited += 10) {
    if (AUDIO::GET::arrived(stream) > 0) return true;
    SDL_Delay(10);
  }
  return AUDIO::GET::arrived(stream) > 0;
}

auto arriving(Whole first, Whole other) -> Flag {
  return settled(first) && settled(other);
}

auto drained(Whole stream) -> Flag {
  Vector<AUDIO::Sample> into(BLOCK * STEREO);
  const Whole took = AUDIO::INPUT::read(stream, into);
  return took > 0 && took <= BLOCK && into.size() == BLOCK * STEREO;
}

auto stopped(Whole stream) -> Flag {
  const Whole before = AUDIO::GET::arrived(stream);
  AUDIO::INPUT::stop(stream);
  if (before == 0 || AUDIO::GET::arrived(stream) >= before) return false;
  return settled(stream);
}

auto missing() -> Flag {
  const Whole stream = BLOCK * BLOCK;
  Vector<AUDIO::Sample> into(STEREO);
  if (AUDIO::INPUT::read(stream, into) != 0) return false;
  AUDIO::INPUT::stop(stream);
  AUDIO::INPUT::remove(stream);
  return AUDIO::GET::arrived(stream) == 0;
}

auto removed(Whole first, Whole other) -> Flag {
  AUDIO::INPUT::remove(first);
  if (AUDIO::GET::arrived(first) != 0) return false;
  if (AUDIO::BACKEND::recorders() == 0) return false;
  AUDIO::INPUT::remove();
  if (AUDIO::GET::arrived(other) != 0 || AUDIO::BACKEND::recorders() != 0)
    return false;
  const Whole fresh = AUDIO::INPUT::create(RATE, MONO);
  const Flag reopened =
    fresh != AUDIO::INPUT::NONE && AUDIO::BACKEND::recorders() == 1;
  AUDIO::INPUT::remove();
  return reopened;
}
