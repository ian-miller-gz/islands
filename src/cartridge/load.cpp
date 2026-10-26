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
#include <cartridge/cartridge.hpp>
#include <cartridge/load.internal.hpp>
#include <console.hpp>
#include <filesystem>
#include <logger.hpp>
#include <string>
#define LOGGER_CATEGORY "~/cartridge::"

String CARTRIDGE::assets;
String CARTRIDGE::cache;
String CARTRIDGE::title;
Map<String, String> CARTRIDGE::keys;
Vector<CARTRIDGE::Requirement> CARTRIDGE::requirements;
Vector<CARTRIDGE::Panel> CARTRIDGE::panels;
Vector<CARTRIDGE::Preset> CARTRIDGE::presets;
Vector<CARTRIDGE::Closure> CARTRIDGE::deferred;
Flag CARTRIDGE::overlay = false;
Flag CARTRIDGE::logs = false;
Flag CARTRIDGE::listen = false;
Flag CARTRIDGE::threads = false;
Flag CARTRIDGE::latch = false;
Float CARTRIDGE::budget = 0;
Flag CARTRIDGE::fed = false;
Flag CARTRIDGE::metrics = true;
Flag CARTRIDGE::decorated = true;
Flag CARTRIDGE::splash = false;
Whole CARTRIDGE::width = 0;
Whole CARTRIDGE::height = 0;
CARTRIDGE::Coverage CARTRIDGE::font = CARTRIDGE::SLIM;
Whole CARTRIDGE::fullscreen = INPUT::BINDINGS::keys["WINDOW::fullscreen"];
Vector<String> CARTRIDGE::web;
Vector<String> CARTRIDGE::server;
Vector<String> CARTRIDGE::components;
const CARTRIDGE::Interface *CARTRIDGE::bound = nullptr;
Flag (*CARTRIDGE::shadowed)(STRING::Hot name) = nullptr;

static void screen(LOGGER::Category &logger) {
  if (!CARTRIDGE::shadowed || !CARTRIDGE::bound->commands) return;
  for (const auto &command : CARTRIDGE::bound->commands())
    if (CARTRIDGE::shadowed(command.name))
      logger.warn(
        "Cartridge command '" + String(command.name) +
        "' shadows an engine command; the engine command wins.");
}

auto CARTRIDGE::load() -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "load");
  CONSOLES::DISPATCH::hotkey =
    INPUT::BINDINGS::keys["CONSOLES::DISPATCH::interpret"];
  fullscreen = INPUT::BINDINGS::keys["WINDOW::fullscreen"];
  if (bind(logger)) {
    unload();
    return 1;
  }
  if (!bound) return 0;
  if (bound->version != VERSION) {
    logger.error(
      "Cartridge version " + std::to_string(bound->version) +
      " does not match engine version " + std::to_string(VERSION) + ".");
    unload();
    return 1;
  }
  if (!cache.empty()) {
    std::error_code error;
    std::filesystem::create_directories(cache, error);
    if (error) {
      logger.error("Cannot create " + cache + ".");
      unload();
      return 1;
    }
  }
  screen(logger);
  logger.info("Cartridge bound.");
  return 0;
}

auto CARTRIDGE::GET::entry() -> const Interface * { return bound; }

auto CARTRIDGE::GET::title() -> String {
  return CARTRIDGE::title.empty() ? ENGINE_NAME : CARTRIDGE::title;
}

void CARTRIDGE::unload() {
  bound = nullptr;
  assets.clear();
  cache.clear();
  title.clear();
  keys.clear();
  requirements.clear();
  panels.clear();
  deferred.clear();
  overlay = false;
  logs = false;
  listen = false;
  threads = false;
  latch = false;
  budget = 0;
  fed = false;
  metrics = true;
  decorated = true;
  splash = false;
  font = SLIM;
  width = 0;
  height = 0;
  web.clear();
  server.clear();
  components.clear();
  CONSOLES::DISPATCH::hotkey =
    INPUT::BINDINGS::keys["CONSOLES::DISPATCH::interpret"];
  fullscreen = INPUT::BINDINGS::keys["WINDOW::fullscreen"];
  release();
}
