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
#include <cstddef>
#include <cstring>
#include <logger.hpp>
#include <network/backend/posix/sessions.internal.hpp>
#define LOGGER_CATEGORY "~/network::sessions"

static auto dial(NETWORK::Descriptor descriptor, const NETWORK::Anchor &anchor)
  -> Flag {
  const String &name = anchor.name;
  sockaddr_un address{};
  address.sun_family = AF_UNIX;
  if (1 + name.size() > sizeof(address.sun_path)) return false;
  address.sun_path[0] = '\0';
  name.copy(address.sun_path + 1, name.size());
  socklen_t span = offsetof(sockaddr_un, sun_path) + 1 + name.size();
  return connect(
           descriptor, reinterpret_cast<const sockaddr *>(&address), span) == 0;
}

auto NETWORK::SESSIONS::create(const Anchor &anchor) -> Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  Descriptor descriptor =
    socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  if (descriptor == CLOSED || !dial(descriptor, anchor)) {
    logger.debug(
      "Cannot connect to @%s: %s", anchor.name.c_str(), std::strerror(errno));
    if (descriptor != CLOSED) ::close(descriptor);
    return NONE;
  }
  logger.debug("Connected to @%s", anchor.name.c_str());
  return adopt(descriptor);
}
