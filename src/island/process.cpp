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
#include <cartridge.hpp>
#include <network.hpp>
#include <island/overlay.hpp>
#include <island/graphics.hpp>
#include <island/input.hpp>
#include <island/process.hpp>
#include <island/process.internal.hpp>
#include <island/reload.hpp>
#include <island/run.hpp>
#include <island/state.hpp>
#include <island/terminal.hpp>
#include <common.hpp>
#include <logger.hpp>
#include <metrics.hpp>
#include <topics.hpp>

static Flag pump();
static Flag poll();
static void prerender();

INPUT::Stamp ISLAND::awaited = 0;

void ISLAND::process() {
  if (!pump()) return;
  if (RUN::threads)
    alternate();
  else
    frame();
}

static Flag pump() {
  using namespace ISLAND;
  METRICS::Scope span("island.pump");
  if (STATE::reload) reload();
  REQUIREMENTS::process();
  RELATIONS::process();
  pace();
  if (!poll()) return false;
  INPUT::process();
  hotkeys();
  RUN::advance();
  return true;
}

void ISLAND::frame() {
  METRICS::frame();
  METRICS::memory();
  METRICS::Scope span("island.frame");
  CLOCK::stamp();
  TOPICS::deliver(TOPICS::now());
  TOPICS::relay();
  prerender();
  {
    METRICS::Scope render("island.render");
    GFX::render();
  }
  awaited = std::chrono::steady_clock::now().time_since_epoch().count();
}

static Flag poll() { return GFX::poll(); }

static void prerender() {
  METRICS::Scope span("island.prerender");
  TERMINAL::process();
  NETWORK::INSTANCES::process();
  OVERLAY::prerender();
}