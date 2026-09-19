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
#include <common.hpp>
#include <logger.hpp>
static const String category = "~/island/audio::";

struct Clip {
  SDL_AudioStream *sdl;
  Vector<Uint8> loop;
};

struct Queued {
  AUDIO::Handle id;
  SDL_AudioSpec spec;
  Vector<Uint8> bytes;
  Flag loop;
  Float gain;
};

static Map<AUDIO::Handle, Clip> clips;
static Vector<Queued> waiting;
static AUDIO::Handle latest = AUDIO::NONE;

static void SDLCALL refill(void *data, SDL_AudioStream *sdl, int more, int) {
  if (more <= 0) return;
  auto *clip = static_cast<Clip *>(data);
  SDL_PutAudioStreamData(
    sdl, clip->loop.data(), static_cast<int>(clip->loop.size()));
}

static void reap() {
  for (auto it = clips.begin(); it != clips.end();)
    if (
      it->second.loop.empty() &&
      SDL_GetAudioStreamQueued(it->second.sdl) <= 0) {
      SDL_DestroyAudioStream(it->second.sdl);
      it = clips.erase(it);
    } else
      ++it;
}

static auto arm(
  AUDIO::Handle id, const SDL_AudioSpec &spec, const void *data, int bytes,
  Flag loop, Float gain) -> Flag {
  auto &logger = LOGGER::get(category + "play");
  auto *sdl = SDL_CreateAudioStream(&spec, &spec);
  if (!sdl || !SDL_BindAudioStreams(AUDIO::BACKEND::playback, &sdl, 1)) {
    logger.error("Play refused: %s", SDL_GetError());
    if (sdl) SDL_DestroyAudioStream(sdl);
    return false;
  }
  auto &clip = clips[id];
  clip.sdl = sdl;
  if (loop) {
    const auto *bytes_at = static_cast<const Uint8 *>(data);
    clip.loop.assign(bytes_at, bytes_at + bytes);
    SDL_SetAudioStreamGetCallback(sdl, refill, &clip);
  }
  if (gain != 1.0f) SDL_SetAudioStreamGain(sdl, gain);
  SDL_PutAudioStreamData(sdl, data, bytes);
  return true;
}

static auto submit(
  const SDL_AudioSpec &spec, const void *data, int bytes,
  Flag loop) -> AUDIO::Handle {
  const AUDIO::Handle id = ++latest;
  if (AUDIO::BACKEND::playback == AUDIO::BACKEND::SILENT) {
    const auto *bytes_at = static_cast<const Uint8 *>(data);
    waiting.push_back(
      {id, spec, Vector<Uint8>(bytes_at, bytes_at + bytes), loop, 1.0f});
    return id;
  }
  reap();
  return arm(id, spec, data, bytes, loop, 1.0f) ? id : AUDIO::NONE;
}

void AUDIO::BACKEND::wake() {
  auto &logger = LOGGER::get(category + "play");
  for (auto &play : waiting)
    if (playback == SILENT)
      logger.error("Play refused: no audio device");
    else
      arm(
        play.id, play.spec, play.bytes.data(),
        static_cast<int>(play.bytes.size()), play.loop, play.gain);
  waiting.clear();
}

auto AUDIO::play(const String &path, Flag loop) -> Handle {
  auto &logger = LOGGER::get(category + "play");
  SDL_AudioSpec spec;
  Uint32 size = 0;
  Uint8 *buffer = nullptr;
  if (!SDL_LoadWAV(path.c_str(), &spec, &buffer, &size)) {
    logger.error("Error loading WAV: %s", SDL_GetError());
    return NONE;
  }
  const Handle clip = submit(spec, buffer, static_cast<int>(size), loop);
  SDL_free(buffer);
  return clip;
}

auto AUDIO::play(
  const Vector<Sample> &samples, Whole rate, Whole channels,
  Flag loop) -> Handle {
  auto &logger = LOGGER::get(category + "play");
  if (!rate || !channels || samples.empty()) {
    logger.error(
      "Play refused: %s",
      !rate || !channels ? "zero in the spec" : "empty buffer");
    return NONE;
  }
  SDL_AudioSpec spec = {
    .format = SDL_AUDIO_S16LE,
    .channels = static_cast<int>(channels),
    .freq = static_cast<int>(rate)};
  return submit(
    spec, samples.data(), static_cast<int>(samples.size() * sizeof(Sample)),
    loop);
}

void AUDIO::volume(Handle clip, Float gain) {
  const auto found = clips.find(clip);
  if (found != clips.end()) {
    SDL_SetAudioStreamGain(found->second.sdl, gain);
    return;
  }
  for (auto &play : waiting)
    if (play.id == clip) {
      play.gain = gain;
      return;
    }
}

void AUDIO::stop(Handle clip) {
  const auto found = clips.find(clip);
  if (found != clips.end()) {
    SDL_DestroyAudioStream(found->second.sdl);
    clips.erase(found);
    return;
  }
  for (auto it = waiting.begin(); it != waiting.end(); ++it)
    if (it->id == clip) {
      waiting.erase(it);
      return;
    }
}

void AUDIO::stop() {
  for (auto &[clip, live] : clips) SDL_DestroyAudioStream(live.sdl);
  clips.clear();
  waiting.clear();
}

auto AUDIO::BACKEND::live() -> Whole {
  return static_cast<Whole>(clips.size());
}

auto AUDIO::BACKEND::pending() -> Whole {
  return static_cast<Whole>(waiting.size());
}

auto AUDIO::BACKEND::backlog(Handle clip) -> long {
  const auto found = clips.find(clip);
  if (found == clips.end()) return -1;
  return static_cast<long>(SDL_GetAudioStreamQueued(found->second.sdl));
}
