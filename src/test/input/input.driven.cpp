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
#include "input.internal.hpp"

namespace {

std::atomic<Whole> pushes = 0;
std::atomic<Whole> widest = 0;

void hear(const AUDIO::Sample *, Whole frames, void *) {
  Whole seen = widest.load();
  while (frames > seen && !widest.compare_exchange_weak(seen, frames)) {
  }
  ++pushes;
}

}  // namespace

auto driven() -> Flag {
  using AUDIO::INPUT::NONE;
  if (AUDIO::INPUT::create(RATE, STEREO, nullptr, nullptr) != NONE)
    return false;
  if (
    AUDIO::INPUT::create(0, STEREO, hear, nullptr) != NONE ||
    AUDIO::INPUT::create(RATE, 0, hear, nullptr) != NONE)
    return false;
  const Whole stream = AUDIO::INPUT::create(RATE, STEREO, hear, nullptr);
  if (stream == NONE) return false;
  for (Whole waited = 0; waited < SETTLE && pushes.load() == 0; waited += 10)
    SDL_Delay(10);
  const Flag drove =
    pushes.load() > 0 && widest.load() <= AUDIO::BACKEND::CHUNK;
  const Flag drains = AUDIO::GET::arrived(stream) == 0;
  AUDIO::INPUT::remove(stream);
  return drove && drains;
}
