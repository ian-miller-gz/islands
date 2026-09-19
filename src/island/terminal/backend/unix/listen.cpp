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
#include <island/terminal/backend/unix/listen.internal.hpp>
#include <logger.hpp>
#include <string>
#define LOGGER_CATEGORY "~/island/terminal::listen"

#if SR_LISTEN == SR_UNIX

void TERMINAL::BACKEND::LISTEN::initialize() {
  if (!on)
    return LOGGER::get(LOGGER_CATEGORY)
      .debug("Listener not requested; no control socket bound.");
  if (listener != NETWORK::NONE) return;
  const auto reached =
    self ? NETWORK::listen(*self)
         : NETWORK::listen(std::to_string(COMMON::PLATFORM::identity()));
  listener = reached.handle;
  self = NETWORK::Wire{reached.wire};
  if (listener == NETWORK::NONE)
    return LOGGER::get(LOGGER_CATEGORY)
      .error("Cannot listen on %s.", reached.wire.c_str());
  announce();
  join();
}

void TERMINAL::BACKEND::LISTEN::process() {
  if (listener == NETWORK::NONE) return;
  publish();
  if (session == NETWORK::NONE) session = NETWORK::LISTENERS::accept(listener);
  if (session != NETWORK::NONE) serve();
  stream();
}

void TERMINAL::BACKEND::LISTEN::close() {
  leave();
  withdraw();
  drop();
  if (listener == NETWORK::NONE) return;
  NETWORK::LISTENERS::destroy(listener);
  listener = NETWORK::NONE;
}

#endif
