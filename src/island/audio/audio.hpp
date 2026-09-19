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

#include <common/types.hpp>

namespace AUDIO {

using Sample = int16_t;

using Handle = Whole;

constexpr Whole LANES = 8;

constexpr Handle NONE = 0;

void initialize();

auto play(const String &path, Flag loop = false) -> Handle;

auto play(
  const Vector<Sample> &samples, Whole rate, Whole channels,
  Flag loop = false) -> Handle;

void volume(Handle clip, Float gain);

void stop(Handle clip);

void stop();

namespace OUTPUT {

constexpr Handle NONE = 0;

struct Device {
  String name;
  Whole channels = 0;
  Whole rate = 0;
};

auto create(Whole rate, Whole channels) -> Handle;

auto create(Whole rate, Whole channels, const String &device) -> Handle;

using Pull = auto (*)(Sample *into, Whole frames, void *listener) -> Whole;

auto create(Whole rate, Whole channels, Pull pull, void *listener) -> Handle;

auto create(
  Whole rate, Whole channels, const String &device, Pull pull,
  void *listener) -> Handle;

void feed(Handle stream, const Vector<Sample> &samples);

void volume(Handle stream, Float gain);

void stop(Handle stream);

void remove(Handle stream);

void remove();

namespace GET {
auto devices() -> Vector<Device>;

auto preferred() -> String;
}  // namespace GET

}  // namespace OUTPUT

namespace INPUT {

constexpr Handle NONE = 0;

struct Device {
  String name;
  Whole channels = 0;
  Whole rate = 0;
};

auto create(Whole rate, Whole channels) -> Handle;

auto create(Whole rate, Whole channels, const String &device) -> Handle;

using Push = void (*)(const Sample *from, Whole frames, void *listener);

auto create(Whole rate, Whole channels, Push push, void *listener) -> Handle;

auto create(
  Whole rate, Whole channels, const String &device, Push push,
  void *listener) -> Handle;

auto read(Handle stream, Vector<Sample> &into) -> Whole;

void stop(Handle stream);

void remove(Handle stream);

void remove();

namespace GET {
auto devices() -> Vector<Device>;
}  // namespace GET

}  // namespace INPUT

namespace GET {
auto queued(Handle stream) -> Whole;

auto arrived(Handle stream) -> Whole;
}  // namespace GET

};  // namespace AUDIO
