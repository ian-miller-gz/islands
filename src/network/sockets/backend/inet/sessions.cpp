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
#include <logger.hpp>
#include <network/sockets/dial.internal.hpp>
#include <network/sockets/inet.internal.hpp>
#include <network/sockets/sessions.internal.hpp>
#define LOGGER_CATEGORY "~/network::sessions"

static auto dial(NETWORK::Descriptor descriptor, const NETWORK::Socket &socket)
  -> Flag {
  NETWORK::INET::Sockaddr address{};
  if (!NETWORK::INET::resolve(socket, address)) return false;
  if (
    connect(
      descriptor, reinterpret_cast<const sockaddr *>(&address),
      sizeof(address)) == 0)
    return true;
  return NETWORK::failing() == NETWORK::PENDING && NETWORK::settled(descriptor);
}

auto NETWORK::SESSIONS::create(const Socket &socket) -> Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  String where =
    INET::OPEN + socket.host + INET::SHUT + std::to_string(socket.port);
  Descriptor descriptor = opened(INET::FAMILY, SOCK_STREAM, INET::PROTOCOL);
  if (descriptor == CLOSED || !dial(descriptor, socket)) {
    logger.debug("Cannot connect to %s: %s", where.c_str(), failure().c_str());
    if (descriptor != CLOSED) close(descriptor);
    return NONE;
  }
  logger.debug("Connected to %s", where.c_str());
  return adopt(descriptor);
}
