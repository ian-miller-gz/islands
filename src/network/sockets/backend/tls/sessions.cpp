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
#include <network/sockets/tls.internal.hpp>
#define LOGGER_CATEGORY "~/network::sessions"

static auto dial(NETWORK::Descriptor descriptor, const NETWORK::Tunnel &tunnel)
  -> Flag {
  NETWORK::INET::Sockaddr address{};
  if (!NETWORK::INET::resolve({tunnel.host, tunnel.port}, address))
    return false;
  if (
    connect(
      descriptor, reinterpret_cast<const sockaddr *>(&address),
      sizeof(address)) == 0)
    return true;
  return NETWORK::failing() == NETWORK::PENDING && NETWORK::settled(descriptor);
}

static auto secured(NETWORK::Descriptor descriptor) -> Flag {
  NETWORK::TLS::Link *link =
    NETWORK::TLS::secure(descriptor, NETWORK::TLS::GET::client());
  if (!link) return false;
  if (
    mbedtls_ssl_set_hostname(&link->ssl, NETWORK::TLS::PEER) != 0 ||
    !NETWORK::TLS::shaken(link)) {
    NETWORK::TLS::discard(link);
    return false;
  }
  return true;
}

auto NETWORK::SESSIONS::create(const Tunnel &tunnel) -> Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  String where =
    INET::OPEN + tunnel.host + INET::SHUT + std::to_string(tunnel.port);
  Descriptor descriptor = opened(INET::FAMILY, SOCK_STREAM, INET::PROTOCOL);
  if (
    descriptor == CLOSED || !dial(descriptor, tunnel) || !secured(descriptor)) {
    logger.debug("Cannot secure %s: %s", where.c_str(), failure().c_str());
    if (descriptor != CLOSED) close(descriptor);
    return NONE;
  }
  logger.debug("Secured to %s", where.c_str());
  return adopt(descriptor, &TLS::CODEC);
}
