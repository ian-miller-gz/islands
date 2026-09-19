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
#pragma once

#include <SDL3/SDL.h>

#include <island/audio/audio.hpp>

namespace AUDIO::BACKEND {
using Device = SDL_AudioDeviceID;
constexpr Device SILENT = 0;
extern Device playback;

constexpr Whole CHUNK = 1024;

auto listening() -> Device;

auto listening(const String &name) -> Device;

auto recorders() -> Whole;

void close(const String &name);

auto sounding(const String &name) -> Device;
void quiet();
void close();

auto bind(Whole rate, Whole channels, Device onto) -> SDL_AudioStream *;

void wake();

auto live() -> Whole;
auto pending() -> Whole;
auto backlog(Handle clip) -> long;
}  // namespace AUDIO::BACKEND
