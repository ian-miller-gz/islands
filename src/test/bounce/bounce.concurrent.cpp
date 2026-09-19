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
#include <thread>

#include "bounce.internal.hpp"

auto BOUNCE::concurrent(
  const AUDIO::PLUGIN::Plug &surface, const Vector<Tape> &tapes) -> Flag {
  const Whole voices = tapes.size();
  Vector<Vector<AUDIO::PLUGIN::Sample>> serial(voices);
  for (Whole voice = 0; voice < voices; ++voice)
    serial[voice] = render(surface, tapes[voice]);

  Vector<Vector<AUDIO::PLUGIN::Sample>> parallel(voices);
  Vector<std::thread> threads;
  for (Whole voice = 0; voice < voices; ++voice)
    threads.emplace_back(
      [&, voice] { parallel[voice] = render(surface, tapes[voice]); });
  for (auto &thread : threads) thread.join();

  for (Whole voice = 0; voice < voices; ++voice)
    if (parallel[voice] != serial[voice]) return false;
  return true;
}
