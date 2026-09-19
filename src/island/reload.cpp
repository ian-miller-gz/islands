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
#include <cartridge.hpp>
#include <island.hpp>
#include <island/run.hpp>
#include <console.hpp>
#include <logger.hpp>
#include <network.hpp>
#include <metrics.hpp>
#include <topics.hpp>
static const String category = "~/island/reload::";

void ISLAND::reload() {
  auto &logger = LOGGER::get(category + "reload");
  STATE::reload = false;
  logger.info("Reloading Cartridge...");
  CARTRIDGE::close();
  INPUT::POINTER::capture(false);
  INPUT::TEXT::focus(false);
  AUDIO::OUTPUT::remove();
  AUDIO::INPUT::remove();
  AUDIO::stop();
  MIDI::close();
  CONSOLES::remove();
  TERMINAL::FOCUS::reset();
  GUI::reset();
  GUI::DIALOG::close();
  TOPICS::reset();
  METRICS::reset();
  TERMINAL::reset();
  NETWORK::INSTANCES::close();
  NETWORK::WEB::SERVER::close();
  GFX::reset();
  REQUIREMENTS::close();
  RELATIONS::close();
  CARTRIDGE::unload();
  if (CARTRIDGE::load()) {
    logger.error("Reload failed. Running without a cartridge.");
  } else if (const auto *bound = CARTRIDGE::GET::entry();
             bound && !bound->frame) {
    logger.error("Cartridge has no frame hook. Running without a cartridge.");
    CARTRIDGE::unload();
  }
  REQUIREMENTS::initialize();
  RUN::readopt();
  CARTRIDGE::initialize();
  logger.info("Complete!");
}
