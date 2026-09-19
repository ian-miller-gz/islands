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

#include <metrics/selection.hpp>

#include <cstdint>

#include <common.hpp>

namespace METRICS {

using Sample = Float;

constexpr Whole HISTORY = 240;

struct Fold {
  Sample min = 0;
  Sample mean = 0;
  Sample max = 0;
  Sample p99 = 0;
  Whole samples = 0;
};

#if SR_METRICS == SR_REGISTRY
void count(STRING::Hot name);

void count(STRING::Hot name, Whole n);

void gauge(STRING::Hot name, Sample value);

void frame();

void memory();

void reset();

void adopt(Flag on);

void arm(Flag on);

struct Scope {
  explicit Scope(STRING::Hot name);
  ~Scope();
  Scope(const Scope &) = delete;
  Scope &operator=(const Scope &) = delete;

  STRING::Hot name;
  int64_t opened;
};
#else
inline void count(STRING::Hot) {}
inline void count(STRING::Hot, Whole) {}
inline void gauge(STRING::Hot, Sample) {}
inline void frame() {}
inline void memory() {}
inline void reset() {}
inline void adopt(Flag) {}
inline void arm(Flag) {}
struct Scope {
  explicit Scope(STRING::Hot) {}
};
#endif

}  // namespace METRICS

namespace METRICS::GET {
#if SR_METRICS == SR_REGISTRY
auto names() -> Vector<String>;

auto series(STRING::Hot name) -> Vector<Sample>;

auto last(STRING::Hot name) -> Sample;

auto fold(STRING::Hot name) -> Fold;

auto armed() -> Flag;
#else
inline auto names() -> Vector<String> { return {}; }
inline auto series(STRING::Hot) -> Vector<Sample> { return {}; }
inline auto last(STRING::Hot) -> Sample { return 0; }
inline auto fold(STRING::Hot) -> Fold { return {}; }
inline auto armed() -> Flag { return false; }
#endif
}  // namespace METRICS::GET
