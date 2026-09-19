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
#include <island/terminal/backend/unix/listen.internal.hpp>
#include <logger.hpp>
#include <roster/roster.hpp>
#include <string>
#define LOGGER_CATEGORY "~/island/terminal::listen"

#if SR_LISTEN == SR_UNIX

void TERMINAL::BACKEND::LISTEN::announce() {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  if (daemon != NETWORK::NONE) return;
  const auto reached =
    roster ? NETWORK::connect(*roster) : NETWORK::connect(ROSTER::NAME);
  daemon = reached.handle;
  roster = NETWORK::Wire{reached.wire};
  if (daemon == NETWORK::NONE)
    return logger.debug("No roster daemon at %s.", reached.wire.c_str());
  auto hello = ROSTER::greet(
    static_cast<Integer>(COMMON::PLATFORM::identity()), ENGINE_NAME,
    CARTRIDGE::GET::bundle(), *self);
  if (!NETWORK::SESSIONS::send(daemon, hello)) return withdraw();
  logger.debug(
    "Registered with the roster daemon at %s.", reached.wire.c_str());
}

void TERMINAL::BACKEND::LISTEN::withdraw() {
  if (daemon == NETWORK::NONE) return;
  NETWORK::SESSIONS::destroy(daemon);
  daemon = NETWORK::NONE;
}

#endif
