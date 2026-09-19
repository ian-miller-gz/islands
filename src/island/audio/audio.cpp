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
#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>

#include <island/audio/audio.hpp>
#include <island/audio/audio.internal.hpp>
#include <common.hpp>
#include <logger.hpp>
static const String category = "~/island/audio::";

AUDIO::BACKEND::Device AUDIO::BACKEND::playback = AUDIO::BACKEND::SILENT;

static constexpr STRING::Hot INCLUDED = "1";

void AUDIO::initialize() {
  auto &logger = LOGGER::get(category + "initialize");
  SDL_SetHint(SDL_HINT_AUDIO_INCLUDE_MONITORS, ::INCLUDED);
  if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
    logger.warn("Failed to open audio device: %s", SDL_GetError());
    BACKEND::wake();
    return;
  }
  BACKEND::playback =
    SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
  if (BACKEND::playback == BACKEND::SILENT)
    logger.warn("Failed to open audio device: %s", SDL_GetError());
  BACKEND::wake();
}

static Map<String, AUDIO::BACKEND::Device> heard;

static auto resolved(const String &name) -> AUDIO::BACKEND::Device {
  if (name.empty()) return SDL_AUDIO_DEVICE_DEFAULT_RECORDING;
  int count = 0;
  SDL_AudioDeviceID *ids = SDL_GetAudioRecordingDevices(&count);
  auto found = AUDIO::BACKEND::SILENT;
  for (int at = 0; ids != nullptr && at < count; at += 1) {
    const char *spelled = SDL_GetAudioDeviceName(ids[at]);
    if (spelled != nullptr && name == spelled) {
      found = ids[at];
      break;
    }
  }
  SDL_free(ids);
  return found;
}

auto AUDIO::BACKEND::listening() -> Device { return listening(""); }

static auto opened(AUDIO::BACKEND::Device want, const char *purpose)
  -> AUDIO::BACKEND::Device {
  const auto native = SDL_OpenAudioDevice(want, nullptr);
  if (native != AUDIO::BACKEND::SILENT) return native;
  SDL_AudioSpec spec = {};
  int frames = 0;
  if (!SDL_GetAudioDeviceFormat(want, &spec, &frames)) return native;
  static constexpr int WIDTHS[] = {8, 2};
  for (const int width : WIDTHS) {
    if (spec.channels <= width) continue;
    SDL_AudioSpec narrowed = {SDL_AUDIO_S16LE, width, spec.freq};
    const auto opened = SDL_OpenAudioDevice(want, &narrowed);
    if (opened == AUDIO::BACKEND::SILENT) continue;
    LOGGER::get(category + purpose)
      .warn(
        "Device opened at %d of its %d lanes: the system refused the wider",
        width, spec.channels);
    return opened;
  }
  return AUDIO::BACKEND::SILENT;
}

auto AUDIO::BACKEND::listening(const String &name) -> Device {
  const auto standing = ::heard.find(name);
  if (standing != ::heard.end()) return standing->second;
  auto &logger = LOGGER::get(category + "listening");
  if (!SDL_WasInit(SDL_INIT_AUDIO)) return SILENT;
  const Device want = ::resolved(name);
  if (want == SILENT) {
    logger.warn("No recording device named %s", name.c_str());
    return SILENT;
  }
  const Device stood = ::opened(want, "listening");
  if (stood == SILENT) {
    logger.warn("Failed to open recording device: %s", SDL_GetError());
    return SILENT;
  }
  ::heard[name] = stood;
  return stood;
}

auto AUDIO::BACKEND::recorders() -> Whole { return ::heard.size(); }

void AUDIO::BACKEND::close(const String &name) {
  const auto standing = ::heard.find(name);
  if (standing == ::heard.end()) return;
  SDL_CloseAudioDevice(standing->second);
  ::heard.erase(standing);
}

void AUDIO::BACKEND::close() {
  for (auto &[name, device] : ::heard) SDL_CloseAudioDevice(device);
  ::heard.clear();
}

static Map<String, AUDIO::BACKEND::Device> sounded;

static auto played(const String &name) -> AUDIO::BACKEND::Device {
  int count = 0;
  SDL_AudioDeviceID *ids = SDL_GetAudioPlaybackDevices(&count);
  auto found = AUDIO::BACKEND::SILENT;
  for (int at = 0; ids != nullptr && at < count; at += 1) {
    const char *spelled = SDL_GetAudioDeviceName(ids[at]);
    if (spelled != nullptr && name == spelled) {
      found = ids[at];
      break;
    }
  }
  SDL_free(ids);
  return found;
}

auto AUDIO::BACKEND::sounding(const String &name) -> Device {
  if (name.empty()) return playback;
  const auto standing = ::sounded.find(name);
  if (standing != ::sounded.end()) return standing->second;
  auto &logger = LOGGER::get(category + "sounding");
  if (!SDL_WasInit(SDL_INIT_AUDIO)) return SILENT;
  const Device want = ::played(name);
  if (want == SILENT) {
    logger.warn("No playback device named %s", name.c_str());
    return SILENT;
  }
  const Device stood = ::opened(want, "sounding");
  if (stood == SILENT) {
    logger.warn("Failed to open playback device: %s", SDL_GetError());
    return SILENT;
  }
  ::sounded[name] = stood;
  return stood;
}

void AUDIO::BACKEND::quiet() {
  for (auto &[name, device] : ::sounded) SDL_CloseAudioDevice(device);
  ::sounded.clear();
}

auto AUDIO::OUTPUT::GET::devices() -> Vector<AUDIO::OUTPUT::Device> {
  Vector<AUDIO::OUTPUT::Device> found;
  if (!SDL_WasInit(SDL_INIT_AUDIO)) return found;
  int count = 0;
  SDL_AudioDeviceID *ids = SDL_GetAudioPlaybackDevices(&count);
  if (ids == nullptr) return found;
  for (int at = 0; at < count; at += 1) {
    const char *name = SDL_GetAudioDeviceName(ids[at]);
    SDL_AudioSpec spec = {};
    int frames = 0;
    if (name == nullptr || !SDL_GetAudioDeviceFormat(ids[at], &spec, &frames))
      continue;
    found.push_back(
      {String(name), static_cast<Whole>(spec.channels),
       static_cast<Whole>(spec.freq)});
  }
  SDL_free(ids);
  return found;
}

auto AUDIO::OUTPUT::GET::preferred() -> String {
  if (BACKEND::playback == BACKEND::SILENT) return {};
  const char *name = SDL_GetAudioDeviceName(BACKEND::playback);
  return name == nullptr ? String() : String(name);
}

auto AUDIO::INPUT::GET::devices() -> Vector<AUDIO::INPUT::Device> {
  Vector<AUDIO::INPUT::Device> found;
  if (!SDL_WasInit(SDL_INIT_AUDIO)) return found;
  int count = 0;
  SDL_AudioDeviceID *ids = SDL_GetAudioRecordingDevices(&count);
  if (ids == nullptr) return found;
  for (int at = 0; at < count; at += 1) {
    const char *name = SDL_GetAudioDeviceName(ids[at]);
    SDL_AudioSpec spec = {};
    int frames = 0;
    if (name == nullptr || !SDL_GetAudioDeviceFormat(ids[at], &spec, &frames))
      continue;
    found.push_back(
      {String(name), static_cast<Whole>(spec.channels),
       static_cast<Whole>(spec.freq)});
  }
  SDL_free(ids);
  return found;
}

auto AUDIO::BACKEND::bind(Whole rate, Whole channels, Device onto)
  -> SDL_AudioStream * {
  SDL_AudioSpec spec = {
    .format = SDL_AUDIO_S16LE,
    .channels = static_cast<int>(channels),
    .freq = static_cast<int>(rate)};
  auto *stream = SDL_CreateAudioStream(&spec, &spec);
  if (stream && SDL_BindAudioStreams(onto, &stream, 1)) return stream;
  if (stream) SDL_DestroyAudioStream(stream);
  return nullptr;
}