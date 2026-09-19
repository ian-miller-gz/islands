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
#include <island/graphics/capture.hpp>
#include <island/run.hpp>
#include <island/state.hpp>
#include <logger.hpp>
#include <chrono>
#include <format>
#include <string>
#define LOGGER_CATEGORY "~/island/run::"

Whole ISLAND::RUN::frames = 0;
Whole ISLAND::RUN::restarts = 0;
Flag ISLAND::RUN::overlay = false;
Flag ISLAND::RUN::threads = false;
Flag ISLAND::RUN::latch = false;
Float ISLAND::RUN::budget = 0;
static Whole elapsed = 0;
static Whole count = 0;
static std::chrono::steady_clock::time_point started;

void ISLAND::RUN::advance() {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "advance");
  if (!count) started = std::chrono::steady_clock::now();
  logger.debug("Frame " + std::to_string(++count));
  if (!frames || ++elapsed < frames) return;
  elapsed = 0;
  if (restarts) {
    restarts -= 1;
    ISLAND::STATE::restart = true;
    return;
  }
  GFX::CAPTURE::armed = !GFX::CAPTURE::path.empty();
  ISLAND::STATE::terminate = true;
}

auto ISLAND::RUN::GET::frame() -> Whole { return count; }

auto ISLAND::RUN::report() -> String {
  if (!count) return "No frames rendered.";
  auto wall = std::chrono::steady_clock::now() - started;
  auto seconds = std::chrono::duration<Float>(wall).count();
  auto step = std::chrono::duration<Float, std::milli>(wall).count() / count;
  return std::format(
    "Rendered {} frames in {:.2f}s ({:.3f} ms/frame{}).", count, seconds, step,
    threads ? ", threaded" : "");
}
