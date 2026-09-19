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
#include <cstdint>

#include <cartridge/plugin.hpp>
#include <common.hpp>

namespace BOUNCE {

constexpr Whole RATE = 48000;
constexpr Whole CHANNELS = 1;

constexpr Whole BLOCK = 100;

struct Tape {
  Vector<AUDIO::PLUGIN::Event> events;
  Whole frames = 0;
  Whole block = BLOCK;
};

auto render(const AUDIO::PLUGIN::Plug &surface, const Tape &tape)
  -> Vector<AUDIO::PLUGIN::Sample>;

auto render(
  const AUDIO::PLUGIN::Plug &surface, const Tape &tape,
  const Vector<AUDIO::PLUGIN::Sample> &signal) -> Vector<AUDIO::PLUGIN::Sample>;

auto concurrent(const AUDIO::PLUGIN::Plug &surface, const Vector<Tape> &tapes)
  -> Flag;

auto contract(const AUDIO::PLUGIN::Plug &surface) -> Status;

auto digest(const Vector<AUDIO::PLUGIN::Sample> &samples) -> uint64_t;

auto check(const char *name, Flag ok) -> Status;

}  // namespace BOUNCE
