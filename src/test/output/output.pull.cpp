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
#include <atomic>
#include <island/audio/audio.internal.hpp>
#include "output.internal.hpp"

namespace {

std::atomic<Whole> pulls = 0;
std::atomic<Whole> widest = 0;
std::atomic<Whole> whole = 0;
std::atomic<Whole> slivers = 0;

constexpr Whole SLIVER = 60;

void widened(std::atomic<Whole> &mark, Whole frames) {
  Whole seen = mark.load();
  while (frames > seen && !mark.compare_exchange_weak(seen, frames)) {
  }
}

void marked(AUDIO::Sample *into, Whole frames) {
  for (Whole index = 0; index < frames * STEREO; ++index) into[index] = 1;
}

auto render(AUDIO::Sample *into, Whole frames, void *) -> Whole {
  ::widened(widest, frames);
  ::marked(into, frames);
  ++pulls;
  whole += frames;
  return frames;
}

auto trickle(AUDIO::Sample *into, Whole frames, void *) -> Whole {
  const Whole made = frames < ::SLIVER ? frames : ::SLIVER;
  ::marked(into, made);
  slivers += made;
  return made;
}

}  // namespace

auto pulled() -> Flag {
  using AUDIO::OUTPUT::NONE;
  if (AUDIO::OUTPUT::create(RATE, STEREO, nullptr, nullptr) != NONE)
    return false;
  if (
    AUDIO::OUTPUT::create(0, STEREO, render, nullptr) != NONE ||
    AUDIO::OUTPUT::create(RATE, 0, render, nullptr) != NONE)
    return false;
  const Whole stream = AUDIO::OUTPUT::create(RATE, STEREO, render, nullptr);
  const Whole short_ = AUDIO::OUTPUT::create(RATE, STEREO, trickle, nullptr);
  if (stream == NONE || short_ == NONE) return false;
  SDL_Delay(200);
  const Flag drove = pulls.load() > 0 && widest.load() <= AUDIO::BACKEND::CHUNK;
  const Flag served = slivers.load() * 4 > whole.load();
  AUDIO::OUTPUT::remove(stream);
  AUDIO::OUTPUT::remove(short_);
  return drove && served;
}
