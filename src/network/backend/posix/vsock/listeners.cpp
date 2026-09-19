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
#include <network/backend/posix/listeners.internal.hpp>
#include <network/backend/posix/vsock.internal.hpp>
#define LOGGER_CATEGORY "~/network::listeners"

static auto bound(
  NETWORK::Descriptor descriptor, const NETWORK::Context &context,
  Whole backlog) -> Flag {
  NETWORK::VSOCK::Sockaddr address{};
  if (!NETWORK::VSOCK::resolve(context, address)) return false;
  return bind(
           descriptor, reinterpret_cast<const sockaddr *>(&address),
           sizeof(address)) == 0 &&
         listen(descriptor, backlog) == 0;
}

auto NETWORK::LISTENERS::create(const Context &context, Whole backlog)
  -> Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  String where =
    std::to_string(context.cid) + ":" + std::to_string(context.port);
  Descriptor descriptor =
    socket(VSOCK::FAMILY, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  if (descriptor == CLOSED || !bound(descriptor, context, backlog)) {
    logger.debug(
      "Cannot listen on %s: %s", where.c_str(), std::strerror(errno));
    if (descriptor != CLOSED) ::close(descriptor);
    return NONE;
  }
  logger.debug("Listening on %s", where.c_str());
  return adopt(descriptor, "");
}
