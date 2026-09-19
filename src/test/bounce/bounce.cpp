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

#include <algorithm>
#include "bounce.internal.hpp"

auto BOUNCE::render(const AUDIO::PLUGIN::Plug &surface, const Tape &tape)
  -> Vector<AUDIO::PLUGIN::Sample> {
  return render(surface, tape, {});
}

auto BOUNCE::render(
  const AUDIO::PLUGIN::Plug &surface, const Tape &tape,
  const Vector<AUDIO::PLUGIN::Sample> &signal)
  -> Vector<AUDIO::PLUGIN::Sample> {
  void *instance = surface.create(RATE, CHANNELS);
  Vector<AUDIO::PLUGIN::Sample> out(tape.frames, 0);
  for (Whole at = 0; at < signal.size() && at < tape.frames; ++at)
    out[at] = signal[at];
  Vector<AUDIO::PLUGIN::Event> window;
  const Whole block = tape.block == 0 ? BLOCK : tape.block;
  for (Whole start = 0; start < tape.frames; start += block) {
    const Whole frames = std::min(block, tape.frames - start);
    window.clear();
    for (const auto &event : tape.events)
      if (event.offset >= start && event.offset < start + frames)
        window.push_back(
          {event.kind, event.offset - start, event.index, event.value});
    AUDIO::PLUGIN::Sample *outputs[CHANNELS] = {out.data() + start};
    surface.render(instance, outputs, frames, window.data(), window.size());
  }
  surface.destroy(instance);
  return out;
}

auto BOUNCE::digest(const Vector<AUDIO::PLUGIN::Sample> &samples) -> uint64_t {
  uint64_t hash = 1469598103934665603ull;
  const auto *bytes = reinterpret_cast<const unsigned char *>(samples.data());
  for (Whole index = 0; index < samples.size() * sizeof(AUDIO::PLUGIN::Sample);
       ++index) {
    hash ^= bytes[index];
    hash *= 1099511628211ull;
  }
  return hash;
}

auto BOUNCE::check(const char *name, Flag ok) -> Status {
  std::printf("%-40s %s\n", name, ok ? "PASS" : "FAIL");
  return ok ? 0 : 1;
}
