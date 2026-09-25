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
#include <network/sockets/inet.internal.hpp>
#include <network/sockets/listeners.internal.hpp>
#include <network/sockets/tls.internal.hpp>
#define LOGGER_CATEGORY "~/network::listeners"

static auto bound(
  NETWORK::Descriptor descriptor, const NETWORK::Tunnel &tunnel,
  Whole backlog) -> Flag {
  int yes = 1;
  setsockopt(
    descriptor, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&yes),
    sizeof(yes));
  NETWORK::INET::Sockaddr address{};
  if (!NETWORK::INET::resolve({tunnel.host, tunnel.port}, address))
    return false;
  return bind(
           descriptor, reinterpret_cast<const sockaddr *>(&address),
           sizeof(address)) == 0 &&
         listen(descriptor, backlog) == 0;
}

static auto secured(NETWORK::Descriptor descriptor) -> NETWORK::Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  NETWORK::TLS::Link *link =
    NETWORK::TLS::secure(descriptor, NETWORK::TLS::GET::server());
  if (!link || !NETWORK::TLS::shaken(link)) {
    logger.debug("Cannot secure an inbound session.");
    if (link) NETWORK::TLS::discard(link);
    NETWORK::close(descriptor);
    return NETWORK::NONE;
  }
  return NETWORK::SESSIONS::adopt(descriptor, &NETWORK::TLS::CODEC);
}

static auto listened(const NETWORK::Tunnel &tunnel, Whole backlog)
  -> NETWORK::Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  String where = NETWORK::INET::OPEN + tunnel.host + NETWORK::INET::SHUT +
                 std::to_string(tunnel.port);
  NETWORK::Descriptor descriptor =
    NETWORK::TLS::GET::server()
      ? NETWORK::opened(
          NETWORK::INET::FAMILY, SOCK_STREAM, NETWORK::INET::PROTOCOL)
      : NETWORK::CLOSED;
  if (descriptor == NETWORK::CLOSED || !bound(descriptor, tunnel, backlog)) {
    logger.debug(
      "Cannot listen on %s: %s", where.c_str(), NETWORK::failure().c_str());
    if (descriptor != NETWORK::CLOSED) NETWORK::close(descriptor);
    return NETWORK::NONE;
  }
  logger.debug("Listening on %s", where.c_str());
  return NETWORK::LISTENERS::adopt(descriptor, "", nullptr, &secured);
}

auto NETWORK::LISTENERS::create(const Tunnel &tunnel, Whole backlog) -> Handle {
  return listened(tunnel, backlog);
}
