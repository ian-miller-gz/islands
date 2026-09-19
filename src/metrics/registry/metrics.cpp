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
#include <metrics/registry/registry.internal.hpp>

auto METRICS::registry() -> Map<String, Series> & {
  static Map<String, Series> series;
  return series;
}

Flag METRICS::on = true;

void METRICS::adopt(Flag gate) { on = gate; }

void METRICS::count(STRING::Hot name, Whole n) {
  if (!on) return;
  auto &series = registry()[name];
  series.pending += static_cast<Sample>(n);
  series.counted = true;
}

void METRICS::count(STRING::Hot name) { count(name, 1); }

void METRICS::gauge(STRING::Hot name, Sample value) {
  if (!on) return;
  registry()[name].push(value);
}

void METRICS::frame() {
  if (!on) return;
  for (auto &[name, series] : registry()) {
    if (!series.counted) continue;
    series.push(series.pending);
    series.pending = 0;
    series.counted = false;
  }
}

void METRICS::reset() {
  registry().clear();
  HARDWARE::reset();
}
