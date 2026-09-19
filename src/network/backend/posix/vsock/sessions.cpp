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
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <logger.hpp>
#include <network/backend/posix/dial.internal.hpp>
#include <network/backend/posix/sessions.internal.hpp>
#include <network/backend/posix/vsock.internal.hpp>
#define LOGGER_CATEGORY "~/network::sessions"

static auto dial(NETWORK::Descriptor descriptor, const NETWORK::Context &peer)
  -> Flag {
  NETWORK::VSOCK::Sockaddr address{};
  if (!NETWORK::VSOCK::resolve(peer, address)) return false;
  if (
    connect(
      descriptor, reinterpret_cast<const sockaddr *>(&address),
      sizeof(address)) == 0)
    return true;
  return errno == EINPROGRESS && NETWORK::settled(descriptor);
}

auto NETWORK::SESSIONS::create(const Context &peer) -> Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  String where = std::to_string(peer.cid) + ":" + std::to_string(peer.port);
  Descriptor descriptor =
    ::socket(VSOCK::FAMILY, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  if (descriptor == CLOSED || !dial(descriptor, peer)) {
    logger.debug(
      "Cannot connect to %s: %s", where.c_str(), std::strerror(errno));
    if (descriptor != CLOSED) ::close(descriptor);
    return NONE;
  }
  logger.debug("Connected to %s", where.c_str());
  return adopt(descriptor);
}
