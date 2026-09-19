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
#include "reed.internal.hpp"

namespace {
constexpr Whole VOICES = 8;
constexpr Whole FRAMES = 500;
constexpr Float GAIN = 0.5f;
constexpr Whole SHORTEST = 40;
constexpr Whole APART = 7;

auto tapes() -> Vector<BOUNCE::Tape> {
  Vector<BOUNCE::Tape> made;
  for (Whole voice = 0; voice < VOICES; ++voice) {
    const Whole period = SHORTEST + voice * APART;
    made.push_back(
      {{{AUDIO::PLUGIN::Event::NOTE_ON, 0, period, GAIN},
        {AUDIO::PLUGIN::Event::NOTE_OFF, FRAMES / 2, period, 0}},
       FRAMES});
  }
  return made;
}
}  // namespace

auto REED::concurrent() -> Flag {
  return BOUNCE::concurrent(instrument(), tapes());
}
