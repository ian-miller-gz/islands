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
#include <thread>
#include <island/graphics/vsync.hpp>
#include <island/process.internal.hpp>
#include <island/run.hpp>
#include <metrics.hpp>

namespace {

constexpr Float MARGIN = 1;

constexpr Float HERTZ = 24;
constexpr Float SECOND = 1000;
constexpr Float SLOWEST = SECOND / HERTZ;

auto budgeted() -> Float {
  if (ISLAND::RUN::budget > 0) return ISLAND::RUN::budget;
  const auto frame = METRICS::GET::fold("island.frame");
  const auto await = METRICS::GET::fold("gfx.await");
  if (!frame.samples || !await.samples) return 0;
  const Float cost = frame.p99 - await.p99 + MARGIN;
  return cost > 0 ? cost : 0;
}

auto period() -> Float {
  if (!GFX::VSYNC::on) return 0;
  const auto fold = METRICS::GET::fold("frame.period");
  if (!fold.samples || fold.mean > SLOWEST) return 0;
  return fold.mean;
}

}  // namespace

void ISLAND::pace() {
  if (!RUN::latch || RUN::frames) return;
  const Float beat = ::period();
  const Float cost = ::budgeted();
  if (!beat || !cost || cost >= beat) return;
  if (!awaited) return;
  const auto held = std::chrono::duration<Float, std::milli>(beat - cost);
  const auto deadline =
    std::chrono::steady_clock::time_point(
      std::chrono::steady_clock::duration(awaited)) +
    std::chrono::duration_cast<std::chrono::steady_clock::duration>(held);
  std::this_thread::sleep_until(deadline);
}
