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
#include <logger.hpp>
#include <metrics.hpp>
#include <csignal>
#include <filesystem>
#include <format>
#include <iostream>
static const String category = "~/island::";

static int initialize();
static int loop();
static void halt(int);

// The exit tally, taken before the close resets the registry: each metric
// series folded the way the shell's metrics command prints it, so a run
// that never opened a console still tells where its frames went (the
// cartridge, the fence, the present).
static void tally(LOGGER::Category &logger) {
  for (const auto &name : METRICS::GET::names()) {
    const auto fold = METRICS::GET::fold(name.c_str());
    logger.debug(std::format(
      "{}: last={:.3f} min={:.3f} avg={:.3f} p99={:.3f} max={:.3f} n={}", name,
      METRICS::GET::last(name.c_str()), fold.min, fold.mean, fold.p99,
      fold.max, fold.samples));
  }
}

int main(int argc, char **argv) {
  if (ENGINE::stated(argc, argv)) return 0;
  ISLAND::RUN::parse(argc, argv);
  if (ISLAND::RUN::dispatch(argv)) return 2;
  int status = initialize();
  std::signal(SIGTERM, halt);
  while (!ISLAND::STATE::terminate && !status) status = loop();
  return status ? status : CARTRIDGE::status;
}

static void halt(int) { ISLAND::STATE::terminate = true; }

static int initialize() {
  try {
    LOGGER::initialize();
  } catch (const std::exception &error) {
    std::cout << "Fatal error initializing logger. " << error.what()
              << std::endl;
    return 1;
  }
  try {
    auto &root = LOGGER::Category::getRoot();
    root.info("Initializing Logger...");
    root.info(LOGGER::GET::path());
    root.info(TIME::stamp());
    auto &logger = LOGGER::get(category + "main.initialize");
    logger.info("Logger initialized.");
    logger.info("Setting working directory to " + COMMON::HOME + "...");
    std::filesystem::current_path(COMMON::HOME);
    logger.info("Working directory set.");
  } catch (const std::exception &error) {
    LOGGER::get(category + "main.initialize").fatal(error.what());
    return 1;
  } catch (...) {
    return 1;
  }
  return 0;
}

static int loop() {
  auto &logger = LOGGER::get(category + "main.routine");
  try {
    logger.info("Initializing process...");
    ISLAND::initialize();
    logger.info("Initialization successful.");
    logger.info("Processing...");
    while (!(ISLAND::STATE::restart || ISLAND::STATE::terminate))
      ISLAND::process();
    logger.info("Processing ceased.");
    tally(logger);
    logger.info("Closing process...");
    ISLAND::close();
    logger.info("Close successful.");
    if (ISLAND::STATE::restart && !ISLAND::STATE::terminate) {
      logger.info("Process will restart.");
      ISLAND::STATE::restart = false;
    } else {
      logger.info("Process terminated.");
      logger.debug(ISLAND::RUN::report());
    }
  } catch (const std::exception &error) {
    logger.fatal(error.what());
    ISLAND::join();
    return 1;
  }
  return 0;
}
