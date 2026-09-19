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
#include <console/console.internal.hpp>
#include <logger.hpp>
#include <string>
#include <utility>
#define LOGGER_CATEGORY "~/console::"

Vector<CONSOLES::Console> CONSOLES::STATE::consoles;

void CONSOLES::submit(Console &console) {
  console.scroll = 0;
  const auto submitted = std::exchange(console.line, {});
  append(console, console.prompt + submitted);
  console.shell(console, submitted);
}

static auto seed(STRING::Hot greeting) -> CONSOLES::Console & {
  auto &console = CONSOLES::STATE::consoles.emplace_back();
  CONSOLES::append(console, greeting);
  return console;
}

void CONSOLES::initialize() {
  auto &console = seed("Islands console: `help` for commands.");
  console.visible = visible;
  console.hotkey = DISPATCH::hotkey;
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "consoles");
  logger.debug("Consoles: " + std::to_string(STATE::consoles.size()));
}

void CONSOLES::close() { STATE::consoles.clear(); }

auto CONSOLES::create() -> Console & {
  auto &console = STATE::consoles.emplace_back();
  console.guest = true;
  return console;
}

void CONSOLES::remove(Console &console) {
  auto &consoles = STATE::consoles;
  for (auto entry = consoles.begin(); entry != consoles.end(); ++entry) {
    if (&*entry != &console) continue;
    consoles.erase(entry);
    return;
  }
}

void CONSOLES::remove() {
  auto &consoles = STATE::consoles;
  for (Whole i = consoles.size(); i > 0; i -= 1)
    if (consoles[i - 1].guest) remove(consoles[i - 1]);
}

auto CONSOLES::GET::consoles() -> Vector<Console> & { return STATE::consoles; }

auto CONSOLES::GET::console() -> Console & { return STATE::consoles.front(); }
