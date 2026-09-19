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
#include <filesystem>
#include <iostream>

#include <cartridge.hpp>
#include <common.hpp>
#include <console.hpp>
#include <logger.hpp>
#include <metrics.hpp>
#include <network.hpp>
#include <topics.hpp>
#include <reef/halt.hpp>
#include <reef/reef.internal.hpp>
static const String category = "~/reef::";

static Flag threads = false;
static Flag staged = false;

static auto parse(int count, char **values) -> Status {
#if SR_CARTRIDGE == SR_NONE
  for (int at = 1; at + 1 < count; at += 2) {
    const String flag = values[at];
    if (flag == "--cartridge") CARTRIDGE::path = values[at + 1];
    if (flag == "--log") LOGGER::FILTERS::add(values[at + 1]);
    if (flag == "--threads") {
      threads = std::stoi(values[at + 1]) != 0;
      staged = true;
    }
  }
  if (CARTRIDGE::configured()) return 0;
  std::cout << "Usage: " << REEF_NAME << " --cartridge <bundle-directory>"
            << std::endl;
#else
  std::cout << "Static delivery does not reach this host." << std::endl;
#endif
  return 1;
}

static auto initialize() -> Status {
  try {
    LOGGER::initialize();
    auto &logger = LOGGER::get(category + "main.initialize");
    logger.info("Logger initialized.");
    std::filesystem::current_path(COMMON::HOME);
    logger.info("Working directory set.");
  } catch (const std::exception &error) {
    std::cout << "Fatal error initializing host. " << error.what() << std::endl;
    return 1;
  }
  return 0;
}

static auto deliver(LOGGER::Category &logger) -> Status {
  if (CARTRIDGE::load()) return 1;
  const auto *bound = CARTRIDGE::GET::entry();
  if (!bound || !bound->serve) {
    logger.error("Cartridge has no serve hook.");
    return 1;
  }
  LOGGER::TAP::adopt(CARTRIDGE::logs);
  NETWORK::WEB::adopt(CARTRIDGE::web, CARTRIDGE::assets);
  NETWORK::WEB::SERVER::adopt(CARTRIDGE::server, CARTRIDGE::assets);
  METRICS::adopt(CARTRIDGE::metrics);
  if (!staged) threads = CARTRIDGE::threads;
  NETWORK::threads = threads;
  REEF::HALT::arm(threads);
  REQUIREMENTS::initialize();
  CARTRIDGE::initialize();
  if (!CARTRIDGE::finished && !CARTRIDGE::listen) {
    logger.error("Cartridge serves but its manifest omits `listen: true`.");
    CARTRIDGE::close();
    CARTRIDGE::unload();
    return 1;
  }
  return 0;
}

static void loop() {
  while (!REEF::HALT::halted()) {
    NETWORK::wait();
    METRICS::frame();
    METRICS::memory();
    METRICS::Scope span("reef.serve");
    REQUIREMENTS::process();
    RELATIONS::process();
    CLOCK::stamp();
    TOPICS::deliver(TOPICS::now());
    TOPICS::relay();
    CARTRIDGE::serve();
  }
}

static void run(LOGGER::Category &logger) {
  if (CARTRIDGE::finished) return;
  logger.info("Serving...");
  if (threads) {
    REEF::detach();
    REEF::HALT::await();
    REEF::join();
  } else {
    loop();
  }
  logger.info("Serving ceased.");
}

static auto serve() -> Status {
  auto &logger = LOGGER::get(category + "main.routine");
  try {
    if (deliver(logger)) return 1;
    run(logger);
    CARTRIDGE::close();
    NETWORK::WEB::SERVER::close();
    TOPICS::reset();
    METRICS::reset();
    REQUIREMENTS::close();
    RELATIONS::close();
    CARTRIDGE::unload();
    logger.info("Process terminated.");
  } catch (const std::exception &error) {
    logger.fatal(error.what());
    return 1;
  }
  return CARTRIDGE::status;
}

int main(int argc, char **argv) {
  if (ENGINE::stated(argc, argv)) return 0;
  if (parse(argc, argv)) return 2;
  if (initialize()) return 1;
  return serve();
}
