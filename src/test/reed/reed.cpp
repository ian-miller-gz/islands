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
#include <cstdint>
#include <cstdio>

#include "reed.internal.hpp"

namespace {
constexpr uint64_t DIGEST = 0x8552293ba888eed5ull;
}  // namespace

int main() {
  const auto &surface = REED::instrument();
  Status status = 0;

  status |= BOUNCE::check(
    "create refuses a zero spec", surface.create(0, 1) == nullptr &&
                                    surface.create(BOUNCE::RATE, 0) == nullptr);

  const BOUNCE::Tape proof = {
    {{AUDIO::PLUGIN::Event::NOTE_ON, 120, 50, 0.5f},
     {AUDIO::PLUGIN::Event::NOTE_OFF, 360, 50, 0}},
    500};
  const auto rendered = BOUNCE::render(surface, proof);
  std::printf(
    "digest %016lx\n", static_cast<unsigned long>(BOUNCE::digest(rendered)));

  status |= BOUNCE::check(
    "render is deterministic", rendered == BOUNCE::render(surface, proof));
  status |=
    BOUNCE::check("blocks hash to the pin", BOUNCE::digest(rendered) == DIGEST);

  Flag before = true, during = false, after = true;
  for (Whole frame = 0; frame < proof.frames; ++frame) {
    const Flag sound = rendered[frame] != 0;
    if (frame < 120)
      before = before && !sound;
    else if (frame < 360)
      during = during || sound;
    else
      after = after && !sound;
  }
  status |= BOUNCE::check("silence before the note-on", before);
  status |= BOUNCE::check("the note sounds while gated", during);
  status |= BOUNCE::check("silence after the note-off", after);

  void *instance = surface.create(BOUNCE::RATE, BOUNCE::CHANNELS);
  AUDIO::PLUGIN::Sample block[BOUNCE::BLOCK];
  AUDIO::PLUGIN::Sample *outputs[BOUNCE::CHANNELS] = {block};
  const AUDIO::PLUGIN::Event on = {AUDIO::PLUGIN::Event::NOTE_ON, 0, 50, 0.5f};
  surface.render(instance, outputs, BOUNCE::BLOCK, &on, 1);
  status |=
    BOUNCE::check("the meter publishes the level", surface.meter(instance) > 0);
  const AUDIO::PLUGIN::Event off = {AUDIO::PLUGIN::Event::NOTE_OFF, 0, 50, 0};
  surface.render(instance, outputs, BOUNCE::BLOCK, &off, 1);
  status |=
    BOUNCE::check("the meter falls silent", surface.meter(instance) == 0);
  surface.destroy(instance);

  status |=
    BOUNCE::check("instances render concurrently (R2)", REED::concurrent());
  return status ? 1 : 0;
}
