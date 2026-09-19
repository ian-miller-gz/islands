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
#include <island.hpp>
#include <cartridge.hpp>
#include <common.hpp>
#include <island/graphics/capture.hpp>
#include <logger.hpp>
#include <emscripten/html5.h>
#include <filesystem>
#include <iostream>
static const String category = "~/island::";

static int initialize();
static EM_BOOL cycle(double, void *);
static EM_BOOL conclude();
static void renew();

namespace {
Flag prepared = false;
}  // namespace

int main(int argc, char **argv) {
  ISLAND::RUN::parse(argc, argv);
  if (initialize()) return 1;
  auto &logger = LOGGER::get(category + "main.routine");
  try {
    logger.info("Initializing process...");
    ISLAND::initialize();
    logger.info("Initialization successful.");
    logger.info("Processing...");
  } catch (const std::exception &error) {
    logger.fatal(error.what());
    return 1;
  }
  emscripten_request_animation_frame_loop(&cycle, nullptr);
  return 0;
}

static int initialize() {
  try {
    LOGGER::initialize();
  } catch (const std::exception &error) {
    std::cout << "Fatal error initializing logger. " << error.what()
              << std::endl;
    return 1;
  }
  auto &root = LOGGER::Category::getRoot();
  root.info("Initializing Logger...");
  root.info(LOGGER::GET::path());
  root.info(TIME::stamp());
  return 0;
}

static EM_BOOL cycle(double, void *) {
  auto &logger = LOGGER::get(category + "main.routine");
  try {
    if (ISLAND::STATE::terminate) {
      if (GFX::CAPTURE::armed) return EM_TRUE;
      return conclude();
    }
    if (ISLAND::STATE::restart) {
      renew();
      return EM_TRUE;
    }
    if (!GFX::ready()) return EM_TRUE;
    if (!prepared) {
      CARTRIDGE::initialize();
      prepared = true;
    }
    ISLAND::process();
  } catch (const std::exception &error) {
    logger.fatal(error.what());
    ISLAND::join();
    return EM_FALSE;
  }
  return EM_TRUE;
}

static EM_BOOL conclude() {
  auto &logger = LOGGER::get(category + "main.routine");
  logger.info("Closing process...");
  ISLAND::close();
  logger.info("Process terminated.");
  logger.debug(ISLAND::RUN::report());
  return EM_FALSE;
}

static void renew() {
  auto &logger = LOGGER::get(category + "main.routine");
  logger.info("Closing process...");
  ISLAND::close();
  ISLAND::STATE::restart = false;
  prepared = false;
  logger.info("Process will restart.");
  ISLAND::initialize();
}
