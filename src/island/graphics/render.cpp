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
#include <island/graphics/render.hpp>
#include <cartridge.hpp>
#include <island/graphics/backend.hpp>
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/text/text.hpp>
#include <island/input.hpp>
#include <island/splash/splash.hpp>
#include <island/terminal.hpp>
#include <metrics.hpp>
#include <chrono>

static auto since(INPUT::Stamp then, INPUT::Stamp now) -> Float {
  return std::chrono::duration<Float, std::milli>(
           std::chrono::steady_clock::duration(now - then))
    .count();
}

static void age() {
  static INPUT::Stamp previous = 0;
  const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
  const auto folded = INPUT::GET::stamped();
  if (folded) METRICS::gauge("input.age", since(folded, now));
  if (previous) METRICS::gauge("frame.period", since(previous, now));
  previous = now;
}

void GFX::render() {
  {
    METRICS::Scope span("island.cartridge");
    CARTRIDGE::frame();
  }
  SPLASH::render();
  TERMINAL::render();
  GFX::TEXT::flush();
  {
    METRICS::Scope span("island.present");
    GFX::BACKEND::render();
  }
  age();
  GFX::PASSES::submit(Vector<GFX::Pass>{});
  GFX::DISPATCHES::submit(Vector<GFX::Dispatch>{});
  GFX::BACKEND::await();
}
