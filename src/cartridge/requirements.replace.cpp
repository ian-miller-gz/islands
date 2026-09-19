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
#include <cartridge/relations.internal.hpp>
#include <cartridge/requirements.internal.hpp>
#include <logger.hpp>
#include <thread>
#define LOGGER_CATEGORY "~/cartridge::requirements"

static constexpr Whole DEADLINE = 5000;
static constexpr Whole INTERVAL = 50;

static auto silent(const NETWORK::Wire &where) -> Flag {
  for (Whole waited = 0; waited < DEADLINE; waited += INTERVAL) {
    auto session = NETWORK::connect(where).handle;
    if (session == NETWORK::NONE) return true;
    NETWORK::SESSIONS::destroy(session);
    std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL));
  }
  return false;
}

auto REQUIREMENTS::supplant(NETWORK::Handle session, const NETWORK::Wire &where)
  -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  const auto instance = NETWORK::SESSIONS::GET::process(session);
  NETWORK::SESSIONS::destroy(session);
  if (!instance) {
    logger.error(
      "Cannot replace: the running instance is not reachable "
      "as a same-user local process.");
    return 1;
  }
  RELATIONS::SPAWN::end(instance);
  if (silent(where)) return 0;
  logger.error(
    "Cannot replace: the running instance did not release "
    "its rendezvous.");
  return 1;
}
