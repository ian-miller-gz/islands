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
#include <common/platform/selection.hpp>
#include <metrics/registry/hardware.internal.hpp>

auto METRICS::HARDWARE::groups() -> Map<String, Group> & {
  static Map<String, Group> all;
  return all;
}

Flag METRICS::HARDWARE::requested = false;
Flag METRICS::HARDWARE::ready = false;

#if SR_PLATFORM == SR_POSIX

#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <iterator>

namespace {

struct Counter {
  const char *suffix;
  uint64_t config;
};
constexpr Counter COUNTERS[] = {
  {".cycles", PERF_COUNT_HW_CPU_CYCLES},
  {".instructions", PERF_COUNT_HW_INSTRUCTIONS},
  {".cache-references", PERF_COUNT_HW_CACHE_REFERENCES},
  {".cache-misses", PERF_COUNT_HW_CACHE_MISSES},
  {".branch-misses", PERF_COUNT_HW_BRANCH_MISSES}};
static_assert(std::size(COUNTERS) == METRICS::HARDWARE::SLOTS);

Flag refused = false;

auto event(uint64_t config, int leader) -> int {
  perf_event_attr attr;
  std::memset(&attr, 0, sizeof(attr));
  attr.type = PERF_TYPE_HARDWARE;
  attr.size = sizeof(attr);
  attr.config = config;
  attr.disabled = leader < 0 ? 1 : 0;
  attr.exclude_kernel = 1;
  attr.exclude_hv = 1;
  attr.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_TOTAL_TIME_ENABLED |
                     PERF_FORMAT_TOTAL_TIME_RUNNING;
  return static_cast<int>(
    syscall(SYS_perf_event_open, &attr, 0, -1, leader, 0UL));
}

auto refuse(const char *why) -> Flag {
  if (!refused)
    std::fprintf(stderr, "metrics: hardware counters unavailable (%s)\n", why);
  refused = true;
  return false;
}

}  // namespace

auto METRICS::HARDWARE::open(Group &group) -> Flag {
  group.fds[0] = event(COUNTERS[0].config, -1);
  if (group.fds[0] < 0) return refuse("perf_event_open refused");
  for (int i = 1; i < SLOTS; ++i) {
    group.fds[i] = event(COUNTERS[i].config, group.fds[0]);
    if (group.fds[i] >= 0) continue;
    for (int j = 0; j < i; ++j) ::close(group.fds[j]);
    return refuse("counter group member refused");
  }
  group.live = true;
  return true;
}

void METRICS::HARDWARE::close(Group &group) {
  if (!group.live) return;
  for (int fd : group.fds) ::close(fd);
  group.live = false;
}

void METRICS::HARDWARE::enable(Group &group) {
  ioctl(group.fds[0], PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
  ioctl(group.fds[0], PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
}

void METRICS::HARDWARE::disable(Group &group) {
  ioctl(group.fds[0], PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
}

void METRICS::HARDWARE::record(STRING::Hot stage, Group &group) {
  uint64_t buffer[3 + SLOTS];
  if (
    read(group.fds[0], buffer, sizeof(buffer)) !=
    static_cast<ssize_t>(sizeof(buffer)))
    return;
  uint64_t enabled = buffer[1], running = buffer[2];
  if (running == 0) return;
  double scale = static_cast<double>(enabled) / static_cast<double>(running);
  String base = stage;
  for (int i = 0; i < SLOTS; ++i)
    METRICS::gauge(
      (base + COUNTERS[i].suffix).c_str(),
      static_cast<METRICS::Sample>(static_cast<double>(buffer[3 + i]) * scale));
}

#else

auto METRICS::HARDWARE::open(Group &) -> Flag { return false; }
void METRICS::HARDWARE::close(Group &) {}
void METRICS::HARDWARE::enable(Group &) {}
void METRICS::HARDWARE::disable(Group &) {}
void METRICS::HARDWARE::record(STRING::Hot, Group &) {}

#endif
