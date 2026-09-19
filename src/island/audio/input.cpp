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
#include <island/audio/input.internal.hpp>
#include <common.hpp>
#include <logger.hpp>
static const String category = "~/island/audio::";

using AUDIO::INPUT::Stream;

static Map<AUDIO::Handle, Stream> streams;
static AUDIO::Handle latest = AUDIO::INPUT::NONE;

auto AUDIO::INPUT::at(Handle stream) -> Stream * {
  const auto found = streams.find(stream);
  return found == streams.end() ? nullptr : &found->second;
}

auto AUDIO::INPUT::refused(Whole rate, Whole channels) -> Flag {
  if (rate && channels) return false;
  LOGGER::get(category + "create").error("Stream refused: zero in the spec");
  return true;
}

auto AUDIO::INPUT::opened(const String &device) -> BACKEND::Device {
  const auto onto = BACKEND::listening(device);
  if (onto == BACKEND::SILENT)
    LOGGER::get(category + "create")
      .error("Stream refused: no recording device");
  return onto;
}

auto AUDIO::INPUT::seat(Stream stream) -> Handle {
  streams[++latest] = std::move(stream);
  return latest;
}

auto AUDIO::INPUT::create(Whole rate, Whole channels) -> Handle {
  return create(rate, channels, "");
}

auto AUDIO::INPUT::create(Whole rate, Whole channels, const String &device)
  -> Handle {
  if (refused(rate, channels)) return NONE;
  const auto onto = opened(device);
  if (onto == BACKEND::SILENT) return NONE;
  auto *sdl = BACKEND::bind(rate, channels, onto);
  if (!sdl) {
    LOGGER::get(category + "create")
      .error("Stream refused: %s", SDL_GetError());
    return NONE;
  }
  Stream stream{sdl, channels};
  stream.device = device;
  return seat(std::move(stream));
}

void AUDIO::INPUT::released(const String &device) {
  for (const auto &[stream, live] : streams)
    if (live.device == device) return;
  BACKEND::close(device);
}

auto AUDIO::INPUT::read(Handle stream, Vector<Sample> &into) -> Whole {
  const auto *live = at(stream);
  if (!live || into.empty()) return 0;
  const int took = SDL_GetAudioStreamData(
    live->sdl, into.data(), static_cast<int>(into.size() * sizeof(Sample)));
  if (took <= 0) return 0;
  return static_cast<Whole>(took) /
         static_cast<Whole>(sizeof(Sample) * live->channels);
}

void AUDIO::INPUT::stop(Handle stream) {
  const auto *live = at(stream);
  if (live) SDL_ClearAudioStream(live->sdl);
}

void AUDIO::INPUT::remove(Handle stream) {
  const auto *live = at(stream);
  if (!live) return;
  const String device = live->device;
  SDL_DestroyAudioStream(live->sdl);
  streams.erase(stream);
  released(device);
}

void AUDIO::INPUT::remove() {
  for (auto &[stream, live] : streams) SDL_DestroyAudioStream(live.sdl);
  streams.clear();
  BACKEND::close();
}

auto AUDIO::GET::arrived(Handle stream) -> Whole {
  const auto *live = AUDIO::INPUT::at(stream);
  if (!live) return 0;
  const auto bytes = SDL_GetAudioStreamAvailable(live->sdl);
  if (bytes <= 0) return 0;
  return static_cast<Whole>(bytes) /
         static_cast<Whole>(sizeof(Sample) * live->channels);
}
