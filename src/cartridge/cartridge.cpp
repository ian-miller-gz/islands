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
#include <logger.hpp>
#include <stdexcept>
#define LOGGER_CATEGORY "~/cartridge::"

void CARTRIDGE::initialize() {
  CLOCK::reset();
  const Interface *bound = GET::entry();
  if (!bound || !bound->initialize) return;
  if (bound->initialize(assets.c_str()))
    throw std::runtime_error("Cartridge initialization failed.");
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "load");
  for (const Panel &panel : panels)
    if (!panel.prerender)
      logger.warn("Window \"" + panel.name + "\" declared but never bound.");
}

void CARTRIDGE::window(STRING::Hot name, void (*prerender)()) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "load");
  for (Panel &panel : panels)
    if (panel.name == name) {
      panel.prerender = prerender;
      logger.info(String("Window \"") + name + "\" bound.");
      return;
    }
  logger.warn(String("No window \"") + name + "\" is declared.");
}

void CARTRIDGE::preset(const String &name, Flag show) {
  for (Panel &panel : panels)
    if (panel.name == name) panel.show = show;
  for (Preset &recorded : presets)
    if (recorded.name == name) {
      recorded.show = show;
      return;
    }
  presets.push_back({name, show});
}

auto CARTRIDGE::GET::asset(STRING::Hot relative) -> String {
  return assets + relative;
}

auto CARTRIDGE::GET::cache() -> String { return CARTRIDGE::cache; }

auto CARTRIDGE::GET::manifest(STRING::Hot key) -> String {
  auto found = keys.find(key);
  return found == keys.end() ? String() : found->second;
}

Status CARTRIDGE::status = 0;
Flag CARTRIDGE::finished = false;

void CARTRIDGE::fail() { status = 1; }

void CARTRIDGE::finish() { finished = true; }

void CARTRIDGE::defer(Closure work) { deferred.push_back(std::move(work)); }

void CARTRIDGE::drain() {
  Vector<Closure> pending;
  pending.swap(deferred);
  for (Closure &work : pending)
    if (work) work();
}

void CARTRIDGE::frame() {
  drain();
  const Interface *bound = GET::entry();
  if (bound && bound->frame) bound->frame();
}

void CARTRIDGE::serve() {
  drain();
  const Interface *bound = GET::entry();
  if (bound && bound->serve) bound->serve();
}

void CARTRIDGE::close() {
  const Interface *bound = GET::entry();
  if (bound && bound->close) bound->close();
}

auto CARTRIDGE::GET::windows() -> const Vector<Window> & {
  static const Vector<Window> none;
  const Interface *bound = entry();
  return bound && bound->windows ? bound->windows() : none;
}

auto CARTRIDGE::GET::panels() -> const Vector<Panel> & {
  return CARTRIDGE::panels;
}

auto CARTRIDGE::GET::commands() -> const Vector<SHELL::Command> & {
  static const Vector<SHELL::Command> none;
  const Interface *bound = entry();
  return bound && bound->commands ? bound->commands() : none;
}
