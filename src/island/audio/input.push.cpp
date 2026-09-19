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

static void SDLCALL source(void *seat, SDL_AudioStream *, int, int) {
  auto *live = static_cast<AUDIO::INPUT::Stream *>(seat);
  const Whole stride = sizeof(AUDIO::Sample) * live->channels;
  const int chunk = static_cast<int>(AUDIO::BACKEND::CHUNK * stride);
  for (;;) {
    const int took =
      SDL_GetAudioStreamData(live->sdl, live->scratch.data(), chunk);
    if (took <= 0) break;
    live->push(
      live->scratch.data(), static_cast<Whole>(took) / stride, live->listener);
    if (took < chunk) break;
  }
}

auto AUDIO::INPUT::create(Whole rate, Whole channels, Push push, void *listener)
  -> Handle {
  return create(rate, channels, "", push, listener);
}

auto AUDIO::INPUT::create(
  Whole rate, Whole channels, const String &device, Push push,
  void *listener) -> Handle {
  if (!push || refused(rate, channels)) return NONE;
  const auto onto = opened(device);
  if (onto == BACKEND::SILENT) return NONE;
  auto *sdl = BACKEND::bind(rate, channels, onto);
  if (!sdl) {
    LOGGER::get(category + "create")
      .error("Stream refused: %s", SDL_GetError());
    return NONE;
  }
  const Handle stream = seat(
    {sdl, channels, push, listener, Vector<Sample>(BACKEND::CHUNK * channels),
     device});
  SDL_SetAudioStreamPutCallback(sdl, source, at(stream));
  return stream;
}
