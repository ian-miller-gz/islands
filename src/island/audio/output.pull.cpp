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
#include <algorithm>
#include <common.hpp>
#include <logger.hpp>
static const String category = "~/island/audio::";

static void SDLCALL
sink(void *seat, SDL_AudioStream *sdl, int additional, int) {
  auto *live = static_cast<AUDIO::OUTPUT::Stream *>(seat);
  const Whole stride = sizeof(AUDIO::Sample) * live->channels;
  for (Whole want = static_cast<Whole>(additional) / stride; want > 0;) {
    const Whole ask = std::min(want, AUDIO::BACKEND::CHUNK);
    const Whole made =
      std::min(ask, live->pull(live->scratch.data(), ask, live->listener));
    if (made == 0) break;
    SDL_PutAudioStreamData(
      sdl, live->scratch.data(), static_cast<int>(made * stride));
    want -= made;
  }
}

auto AUDIO::OUTPUT::create(
  Whole rate, Whole channels, Pull pull, void *listener) -> Handle {
  return create(rate, channels, "", pull, listener);
}

auto AUDIO::OUTPUT::create(
  Whole rate, Whole channels, const String &device, Pull pull,
  void *listener) -> Handle {
  if (refused(rate, channels) || !pull) return NONE;
  const auto onto = BACKEND::sounding(device);
  if (onto == BACKEND::SILENT) return NONE;
  auto *sdl = BACKEND::bind(rate, channels, onto);
  if (!sdl) {
    LOGGER::get(category + "create")
      .error("Stream refused: %s", SDL_GetError());
    return NONE;
  }
  const Handle stream = seat(
    {sdl, channels, pull, listener, Vector<Sample>(BACKEND::CHUNK * channels)});
  SDL_SetAudioStreamGetCallback(sdl, sink, at(stream));
  return stream;
}
