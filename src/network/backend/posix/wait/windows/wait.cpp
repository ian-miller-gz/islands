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
#include <network/backend/selection.hpp>
#if SR_NETWORK_BACKEND != SR_NONE
#include <network/backend/posix/listeners.internal.hpp>
#include <network/backend/posix/sessions.internal.hpp>
#include <network/wait.hpp>

Flag NETWORK::threads = false;

namespace {
constexpr Byte RING = 1;

struct Bell {
  NETWORK::Descriptor descriptor = NETWORK::CLOSED;
  sockaddr_in address{};
};

auto seated() -> Bell {
  Bell fresh;
  fresh.address.sin_family = AF_INET;
  fresh.address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  fresh.descriptor = NETWORK::opened(AF_INET, SOCK_DGRAM, 0);
  if (fresh.descriptor == NETWORK::CLOSED) return fresh;
  auto *seat = reinterpret_cast<sockaddr *>(&fresh.address);
  int size = sizeof(fresh.address);
  if (
    ::bind(fresh.descriptor, seat, size) != 0 ||
    ::getsockname(fresh.descriptor, seat, &size) != 0) {
    NETWORK::close(fresh.descriptor);
    fresh.descriptor = NETWORK::CLOSED;
  }
  return fresh;
}

auto bell() -> Bell & {
  static Bell rung = seated();
  return rung;
}

void gather(Vector<pollfd> &queries, NETWORK::Descriptor descriptor) {
  if (descriptor == NETWORK::CLOSED) return;
  queries.push_back({.fd = descriptor, .events = POLLIN, .revents = 0});
}
}  // namespace

void NETWORK::wait() {
  Vector<pollfd> queries;
  gather(queries, bell().descriptor);
  for (const auto &entry : LISTENERS::entries)
    gather(queries, entry.descriptor);
  for (auto descriptor : SESSIONS::descriptors) gather(queries, descriptor);
  if (queries.empty()) return;
  poll(queries.data(), queries.size(), -1);
  Byte tick;
  static_cast<void>(
    ::recvfrom(bell().descriptor, &tick, sizeof tick, 0, nullptr, nullptr));
}

void NETWORK::wake() {
  const Bell &rung = bell();
  if (rung.descriptor == CLOSED) return;
  static_cast<void>(::sendto(
    rung.descriptor, &RING, sizeof RING, 0,
    reinterpret_cast<const sockaddr *>(&rung.address), sizeof(rung.address)));
}
#endif
