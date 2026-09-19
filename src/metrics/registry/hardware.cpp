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

#include <metrics/registry/hardware.internal.hpp>

namespace {

constexpr int64_t INTERVAL = 1'000'000'000;

auto now() -> int64_t {
  return std::chrono::steady_clock::now().time_since_epoch().count();
}

auto due(METRICS::HARDWARE::Group &group) -> Flag {
  auto stamp = now();
  if (group.primed && stamp - group.last < INTERVAL) return false;
  group.last = stamp;
  group.primed = true;
  return true;
}

}  // namespace

void METRICS::arm(Flag on_) {
  if (!on_) {
    HARDWARE::requested = false;
    HARDWARE::reset();
    return;
  }
  HARDWARE::Group probe;
  if (!HARDWARE::open(probe)) {
    HARDWARE::requested = HARDWARE::ready = false;
    return;
  }
  HARDWARE::close(probe);
  HARDWARE::requested = HARDWARE::ready = true;
}

auto METRICS::GET::armed() -> Flag {
  return HARDWARE::requested && HARDWARE::ready;
}

void METRICS::HARDWARE::begin(STRING::Hot stage) {
  if (!requested || !on) return;
  Group &group = groups()[stage];
  if (!group.live && !open(group)) {
    requested = ready = false;
    return;
  }
  if (!due(group)) return void(group.capturing = false);
  enable(group);
  group.capturing = true;
}

void METRICS::HARDWARE::end(STRING::Hot stage) {
  if (!requested || !on) return;
  auto it = groups().find(stage);
  if (it == groups().end() || !it->second.capturing) return;
  disable(it->second);
  it->second.capturing = false;
  record(stage, it->second);
}

void METRICS::HARDWARE::reset() {
  for (auto &[stage, group] : groups()) close(group);
  groups().clear();
}
