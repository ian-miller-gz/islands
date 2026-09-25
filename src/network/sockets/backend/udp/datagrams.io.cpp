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
#include <network/sockets/backend/udp/datagrams.internal.hpp>

static constexpr Whole CHUNK = 2048;
static constexpr Integer PATIENCE = 1000;

auto NETWORK::DATAGRAM::emit(
  Handle endpoint, const Socket &to, const String &data) -> Flag {
  Descriptor descriptor = fetch(endpoint);
  sockaddr_in address{};
  if (descriptor == CLOSED || !encode(to, address)) return false;
  return sendto(
           descriptor, data.data(), static_cast<int>(data.size()), QUIET,
           reinterpret_cast<const sockaddr *>(&address), sizeof(address)) >= 0;
}

auto NETWORK::DATAGRAM::receive(Handle endpoint, Socket &from, String &data)
  -> Flag {
  Descriptor descriptor = fetch(endpoint);
  if (descriptor == CLOSED) return false;
  pollfd query{.fd = descriptor, .events = POLLIN, .revents = 0};
  if (NETWORK::poll(&query, 1, PATIENCE) <= 0) return false;
  char chunk[CHUNK];
  sockaddr_in address{};
  socklen_t size = sizeof(address);
  auto got = recvfrom(
    descriptor, chunk, sizeof(chunk), 0, reinterpret_cast<sockaddr *>(&address),
    &size);
  if (got < 0) return false;
  char host[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &address.sin_addr, host, sizeof(host));
  from = {host, ntohs(address.sin_port)};
  data.assign(chunk, got);
  return true;
}
#endif
