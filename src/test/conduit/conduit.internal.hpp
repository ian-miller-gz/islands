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

#include <conduit.hpp>

constexpr Whole WIDTH = 4;
constexpr uint64_t RUN = 10000;
constexpr uint64_t SPIN = 100000000ull;

struct Cell {
  uint64_t seq = 0;
  uint64_t sum = 0;
};
constexpr Whole STRIDE = sizeof(Cell);

auto named(const char *base) -> String;

auto sealed(uint64_t seq) -> Cell;

auto intact(const Cell &cell, uint64_t seq) -> Flag;

auto produce(const String &name) -> Status;

auto drained(CONDUIT::Handle owner) -> Flag;

auto wire() -> Flag;
