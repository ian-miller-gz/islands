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
#include <shell/engine/engine.internal.hpp>
#include <metrics.hpp>
#include <format>

static void hardware(SHELL::Session &session) {
  const auto &arguments = session.arguments;
  if (arguments.size() >= 3 && arguments[2] == "on") {
    METRICS::arm(true);
    return session.print(
      METRICS::GET::armed() ? "Hardware counters armed."
                            : "Hardware counters unavailable "
                              "(perf_event_open refused).");
  }
  if (arguments.size() >= 3 && arguments[2] == "off") {
    METRICS::arm(false);
    return session.print("Hardware counters disarmed.");
  }
  session.print(
    METRICS::GET::armed() ? "Hardware counters armed."
                          : "Hardware counters off.");
}

void SHELL::ENGINE::ACTIONS::metrics(Session &session) {
  if (session.arguments.size() >= 2 && session.arguments[1] == "hw")
    return hardware(session);
  if (host.summary) host.summary(session);
  auto names = METRICS::GET::names();
  if (names.empty()) return session.print("No metrics recorded.");
  for (const auto &name : names) {
    auto fold = METRICS::GET::fold(name.c_str());
    session.print(std::format(
      "{}: last={:.3f} min={:.3f} avg={:.3f} p99={:.3f} max={:.3f} n={}", name,
      METRICS::GET::last(name.c_str()), fold.min, fold.mean, fold.p99, fold.max,
      fold.samples));
  }
}
