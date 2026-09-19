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
#include <cstdio>
#include <fstream>
#include "clips.internal.hpp"

static void write(
  const char *path, Whole rate, Whole channels,
  const Vector<AUDIO::Sample> &samples) {
  const Whole bytes = samples.size() * sizeof(AUDIO::Sample);
  const Whole align = channels * sizeof(AUDIO::Sample);
  auto le = [](std::ofstream &out, Whole value, Whole width) {
    for (Whole i = 0; i < width; ++i) out.put((value >> (8 * i)) & 0xff);
  };
  std::ofstream out(path, std::ios::binary);
  out.write("RIFF", 4), le(out, 36 + bytes, 4), out.write("WAVE", 4);
  out.write("fmt ", 4), le(out, 16, 4), le(out, 1, 2), le(out, channels, 2);
  le(out, rate, 4), le(out, rate * align, 4);
  le(out, align, 2), le(out, 8 * sizeof(AUDIO::Sample), 2);
  out.write("data", 4), le(out, bytes, 4);
  out.write(reinterpret_cast<const char *>(samples.data()), bytes);
}

auto honored(AUDIO::Handle &clip) -> Flag {
  const char *path = "clips.fixture.wav";
  write(path, 8000, MONO, Vector<AUDIO::Sample>(BLOCK));
  clip = AUDIO::play(path);
  remove(path);
  return clip != AUDIO::NONE && AUDIO::BACKEND::backlog(clip) > 0;
}
