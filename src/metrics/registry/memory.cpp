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
#include <metrics/registry/memory.internal.hpp>

#include <algorithm>
#include <chrono>

#include <metrics/registry/registry.internal.hpp>

namespace {

constexpr int64_t INTERVAL = 1'000'000'000;
constexpr METRICS::Sample MEBI = 1024.0f * 1024.0f;

auto due() -> Flag {
  static int64_t last = 0;
  static Flag primed = false;
  auto now = std::chrono::steady_clock::now().time_since_epoch().count();
  if (primed && now - last < INTERVAL) return false;
  last = now;
  primed = true;
  return true;
}

}  // namespace

void METRICS::memory() {
  if (!on) return;
  if (!due()) return;
  Sample rss = resident() / MEBI;
  if (rss <= 0) return;
  gauge("memory.rss", rss);
  gauge("memory.peak", std::max(rss, GET::last("memory.peak")));
}
