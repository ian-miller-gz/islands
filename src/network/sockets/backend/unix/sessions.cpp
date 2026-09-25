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
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <logger.hpp>
#include <network/sockets/sessions.internal.hpp>
#define LOGGER_CATEGORY "~/network::sessions"

static auto dial(
  NETWORK::Descriptor descriptor, const NETWORK::Endpoint &endpoint) -> Flag {
  sockaddr_un address{};
  address.sun_family = AF_UNIX;
  if (endpoint.path.size() >= sizeof(address.sun_path)) return false;
  endpoint.path.copy(address.sun_path, endpoint.path.size());
  return connect(
           descriptor, reinterpret_cast<const sockaddr *>(&address),
           sizeof(address)) == 0;
}

auto NETWORK::SESSIONS::create(const Endpoint &endpoint) -> Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  Descriptor descriptor =
    socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  if (descriptor == CLOSED || !dial(descriptor, endpoint)) {
    logger.debug(
      "Cannot connect to %s: %s", endpoint.path.c_str(), std::strerror(errno));
    if (descriptor != CLOSED) ::close(descriptor);
    return NONE;
  }
  logger.debug("Connected to %s", endpoint.path.c_str());
  return adopt(descriptor);
}
