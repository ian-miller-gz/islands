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
#include "output.internal.hpp"

auto fed(Whole stream) -> Flag {
  if (AUDIO::GET::queued(stream) != 0) return false;
  AUDIO::OUTPUT::feed(stream, Vector<AUDIO::Sample>(SECOND * STEREO));
  AUDIO::OUTPUT::volume(stream, HALF);
  const Whole frames = AUDIO::GET::queued(stream);
  return frames > 0 && frames <= SECOND;
}

auto mixed(Whole first, Whole other) -> Flag {
  AUDIO::OUTPUT::feed(other, Vector<AUDIO::Sample>(SLOW));
  const Whole frames = AUDIO::GET::queued(other);
  return frames > 0 && frames <= SLOW && AUDIO::GET::queued(first) > 0;
}

auto stopped(Whole stream) -> Flag {
  AUDIO::OUTPUT::stop(stream);
  if (AUDIO::GET::queued(stream) != 0) return false;
  AUDIO::OUTPUT::feed(stream, Vector<AUDIO::Sample>(SECOND * STEREO));
  return AUDIO::GET::queued(stream) > 0;
}

auto missing() -> Flag {
  const Whole stream = SECOND;
  AUDIO::OUTPUT::feed(stream, Vector<AUDIO::Sample>(STEREO));
  AUDIO::OUTPUT::volume(stream, HALF);
  AUDIO::OUTPUT::stop(stream);
  AUDIO::OUTPUT::remove(stream);
  return AUDIO::GET::queued(stream) == 0;
}

auto removed(Whole first, Whole other) -> Flag {
  AUDIO::OUTPUT::remove(first);
  AUDIO::OUTPUT::feed(first, Vector<AUDIO::Sample>(STEREO));
  if (AUDIO::GET::queued(first) != 0 || AUDIO::GET::queued(other) == 0)
    return false;
  AUDIO::OUTPUT::remove();
  if (AUDIO::GET::queued(other) != 0) return false;
  const Whole fresh = AUDIO::OUTPUT::create(RATE, MONO);
  const Flag reopened = fresh != AUDIO::OUTPUT::NONE;
  AUDIO::OUTPUT::remove();
  return reopened;
}
