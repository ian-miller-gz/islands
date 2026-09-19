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
#include <island/audio/audio.hpp>
#include <island/audio/audio.internal.hpp>
#include <island/audio/output.internal.hpp>
#include <common.hpp>
#include <logger.hpp>
static const String category = "~/island/audio::";

using AUDIO::OUTPUT::Stream;

static Map<AUDIO::Handle, Stream> streams;
static AUDIO::Handle latest = AUDIO::OUTPUT::NONE;

auto AUDIO::OUTPUT::at(Handle stream) -> Stream * {
  const auto found = streams.find(stream);
  return found == streams.end() ? nullptr : &found->second;
}

auto AUDIO::OUTPUT::refused(Whole rate, Whole channels) -> Flag {
  if (BACKEND::playback != BACKEND::SILENT && rate && channels) return false;
  LOGGER::get(category + "create")
    .error(
      "Stream refused: %s", BACKEND::playback == BACKEND::SILENT
                              ? "no audio device"
                              : "zero in the spec");
  return true;
}

auto AUDIO::OUTPUT::seat(Stream stream) -> Handle {
  streams[++latest] = std::move(stream);
  return latest;
}

auto AUDIO::OUTPUT::create(Whole rate, Whole channels) -> Handle {
  return create(rate, channels, "");
}

auto AUDIO::OUTPUT::create(Whole rate, Whole channels, const String &device)
  -> Handle {
  if (refused(rate, channels)) return NONE;
  const auto onto = BACKEND::sounding(device);
  if (onto == BACKEND::SILENT) return NONE;
  auto *sdl = BACKEND::bind(rate, channels, onto);
  if (!sdl) {
    LOGGER::get(category + "create")
      .error("Stream refused: %s", SDL_GetError());
    return NONE;
  }
  return seat({sdl, channels});
}

void AUDIO::OUTPUT::feed(Handle stream, const Vector<Sample> &samples) {
  const auto *live = at(stream);
  if (live)
    SDL_PutAudioStreamData(
      live->sdl, samples.data(),
      static_cast<int>(samples.size() * sizeof(Sample)));
}

void AUDIO::OUTPUT::volume(Handle stream, Float gain) {
  const auto *live = at(stream);
  if (live) SDL_SetAudioStreamGain(live->sdl, gain);
}

void AUDIO::OUTPUT::stop(Handle stream) {
  const auto *live = at(stream);
  if (live) SDL_ClearAudioStream(live->sdl);
}

void AUDIO::OUTPUT::remove(Handle stream) {
  const auto *live = at(stream);
  if (!live) return;
  SDL_DestroyAudioStream(live->sdl);
  streams.erase(stream);
}

void AUDIO::OUTPUT::remove() {
  for (auto &[stream, live] : streams) SDL_DestroyAudioStream(live.sdl);
  streams.clear();
  BACKEND::quiet();
}

auto AUDIO::GET::queued(Handle stream) -> Whole {
  const auto *live = AUDIO::OUTPUT::at(stream);
  if (!live) return 0;
  const auto bytes = SDL_GetAudioStreamQueued(live->sdl);
  if (bytes <= 0) return 0;
  return static_cast<Whole>(bytes) /
         static_cast<Whole>(sizeof(Sample) * live->channels);
}
