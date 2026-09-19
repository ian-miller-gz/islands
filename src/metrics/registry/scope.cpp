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
#include <chrono>

#include <metrics/registry/registry.internal.hpp>

static auto stamp() -> int64_t {
  return std::chrono::steady_clock::now().time_since_epoch().count();
}

METRICS::Scope::Scope(STRING::Hot name) : name(name), opened(0) {
  HARDWARE::begin(name);
  opened = stamp();
}

METRICS::Scope::~Scope() {
  if (!on) return;
  auto nanos = static_cast<Sample>(stamp() - opened);
  registry()[name].push(nanos / 1'000'000.0f);
  HARDWARE::end(name);
}
