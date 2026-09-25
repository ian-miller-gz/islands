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
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <logger.hpp>
#include <network/sockets/listeners.internal.hpp>
#define LOGGER_CATEGORY "~/network::listeners"

static constexpr Whole MODE = 0700;

static void prepare(const String &where) {
  auto cut = where.rfind('/');
  if (cut != String::npos && cut) mkdir(where.substr(0, cut).c_str(), MODE);
}

static Flag stale(const String &where) {
  struct stat status;
  if (lstat(where.c_str(), &status) != 0) return errno == ENOENT;
  return S_ISSOCK(status.st_mode);
}

static Flag bound(
  NETWORK::Descriptor descriptor, const NETWORK::Endpoint &endpoint,
  Whole backlog) {
  const String &where = endpoint.path;
  sockaddr_un address{};
  address.sun_family = AF_UNIX;
  if (where.size() >= sizeof(address.sun_path)) return false;
  where.copy(address.sun_path, where.size());
  prepare(where);
  if (!stale(where)) return false;
  unlink(where.c_str());
  return bind(
           descriptor, reinterpret_cast<const sockaddr *>(&address),
           sizeof(address)) == 0 &&
         listen(descriptor, backlog) == 0;
}

static void unlinked(const String &token) { unlink(token.c_str()); }

auto NETWORK::LISTENERS::create(const Endpoint &endpoint, Whole backlog)
  -> Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  Descriptor descriptor =
    socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  if (descriptor == CLOSED || !bound(descriptor, endpoint, backlog)) {
    logger.debug(
      "Cannot listen on %s: %s", endpoint.path.c_str(), std::strerror(errno));
    if (descriptor != CLOSED) ::close(descriptor);
    return NONE;
  }
  logger.debug("Listening on %s", endpoint.path.c_str());
  return adopt(descriptor, endpoint.path, &unlinked);
}
