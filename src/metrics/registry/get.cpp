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
#include <algorithm>
#include <cmath>
#include <numeric>

#include <metrics/registry/registry.internal.hpp>

static auto find(STRING::Hot name) -> const METRICS::Series * {
  auto &series = METRICS::registry();
  auto it = series.find(name);
  return it == series.end() ? nullptr : &it->second;
}

auto METRICS::GET::names() -> Vector<String> {
  Vector<String> all;
  for (const auto &[name, series] : registry()) all.push_back(name);
  std::sort(all.begin(), all.end());
  return all;
}

auto METRICS::GET::series(STRING::Hot name) -> Vector<Sample> {
  const auto *found = find(name);
  if (!found) return {};
  return {found->window.begin(), found->window.end()};
}

auto METRICS::GET::last(STRING::Hot name) -> Sample {
  const auto *found = find(name);
  if (!found || found->window.empty()) return 0;
  return found->window.back();
}

auto METRICS::GET::fold(STRING::Hot name) -> Fold {
  const auto *found = find(name);
  if (!found || found->window.empty()) return {};
  Vector<Sample> sorted(found->window.begin(), found->window.end());
  std::sort(sorted.begin(), sorted.end());
  Fold fold;
  fold.samples = static_cast<Whole>(sorted.size());
  fold.min = sorted.front();
  fold.max = sorted.back();
  fold.mean = std::accumulate(sorted.begin(), sorted.end(), Sample{0}) /
              static_cast<Sample>(fold.samples);
  auto rank = static_cast<Whole>(std::ceil(0.99f * fold.samples));
  fold.p99 = sorted[std::min(fold.samples, rank) - 1];
  return fold;
}
