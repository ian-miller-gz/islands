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
#include <common.hpp>
#include <chrono>

namespace {
using Instant = std::chrono::steady_clock::time_point;

Flag started = false;
Whole counter = 0;
Instant origin;
Instant previous;
Float span = 0.0f;
Float lifetime = 0.0f;
Float smoothed = 0.0f;

constexpr Float SMOOTHING = 0.1f;

auto seconds(Instant from, Instant to) -> Float {
  return std::chrono::duration<Float>(to - from).count();
}
}  // namespace

void CLOCK::reset() {
  started = false;
  counter = 0;
  span = 0.0f;
  lifetime = 0.0f;
  smoothed = 0.0f;
}

void CLOCK::stamp() {
  const auto now = std::chrono::steady_clock::now();
  if (!started) {
    started = true;
    origin = now;
    previous = now;
    return;
  }
  span = seconds(previous, now);
  lifetime = seconds(origin, now);
  previous = now;
  counter += 1;
  if (span > 0.0f) {
    const Float rate = 1.0f / span;
    smoothed =
      smoothed == 0.0f ? rate : smoothed + SMOOTHING * (rate - smoothed);
  }
}

auto CLOCK::GET::delta() -> Float { return span; }
auto CLOCK::GET::elapsed() -> Float { return lifetime; }
auto CLOCK::GET::frame() -> Whole { return counter; }
auto CLOCK::GET::rate() -> Float { return smoothed; }

auto CLOCK::sequence(Whole count, Float period) -> Whole {
  if (count == 0 || period <= 0.0f) return 0;
  return static_cast<Whole>(lifetime / period) % count;
}

auto CLOCK::pulse(Float period) -> Flag {
  return period > 0.0f && sequence(2, period) == 0;
}
