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
#include <island/audio/audio.hpp>

constexpr Whole RATE = 16000;
constexpr Whole SLOW = 8000;
constexpr Whole STEREO = 2;
constexpr Whole MONO = 1;
constexpr Whole BLOCK = 256;
constexpr Whole SETTLE = 200;

auto check(const char *name, Flag ok) -> Status;

auto arriving(Whole first, Whole other) -> Flag;
auto drained(Whole stream) -> Flag;
auto stopped(Whole stream) -> Flag;
auto missing() -> Flag;
auto removed(Whole first, Whole other) -> Flag;

auto driven() -> Flag;
