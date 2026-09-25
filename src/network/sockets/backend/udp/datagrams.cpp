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
#include <network/selection.hpp>
#if SR_DATAGRAM_BACKEND == SR_UDP
#include <logger.hpp>
#include <network/sockets/backend/udp/datagrams.internal.hpp>
#define LOGGER_CATEGORY "~/network::datagrams"

auto NETWORK::DATAGRAM::open(const Socket &local) -> Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  Descriptor descriptor = opened(AF_INET, SOCK_DGRAM, 0);
  sockaddr_in address{};
  if (
    descriptor == CLOSED || !encode(local, address) ||
    bind(
      descriptor, reinterpret_cast<const sockaddr *>(&address),
      sizeof(address)) != 0) {
    logger.debug("Cannot open datagram endpoint: %s", failure().c_str());
    if (descriptor != CLOSED) NETWORK::close(descriptor);
    return NONE;
  }
  for (Whole slot = 0; slot < endpoints.size(); ++slot)
    if (endpoints[slot] == CLOSED) return endpoints[slot] = descriptor, slot;
  return endpoints.push_back(descriptor), endpoints.size() - 1;
}

void NETWORK::DATAGRAM::close(Handle endpoint) {
  if (!bound(endpoint)) return;
  NETWORK::close(endpoints[endpoint]);
  endpoints[endpoint] = CLOSED;
}
#endif
