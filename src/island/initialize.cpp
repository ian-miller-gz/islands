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
#include <island/audio.hpp>
#include <island/splash/splash.hpp>
#include <logger.hpp>
#include <stdexcept>
static const String category = "~/island/initialize::";

void ISLAND::initialize() {
  auto &logger = LOGGER::get(category + "initialize");
  logger.info("Loading Cartridge...");
  if (CARTRIDGE::load()) throw std::runtime_error("Cartridge load failed.");
  if (const auto *bound = CARTRIDGE::GET::entry(); bound && !bound->frame)
    throw std::runtime_error("Cartridge has no frame hook.");
  ISLAND::RUN::adopt();
  logger.info("Complete!");
  REQUIREMENTS::initialize();
  logger.info("Initializing Terminal...");
  TERMINAL::initialize();
  logger.info("Complete!");
  logger.info("Initializing Audio...");
  AUDIO::initialize();
  logger.info("Complete!");
  logger.info("Initializing Graphics...");
  GFX::initialize();
  logger.info("Complete!");
  logger.info("Initializing Overlay...");
  OVERLAY::initialize();
  logger.info("Complete!");
  SPLASH::prepare();
  if (RUN::threads) detach();
}