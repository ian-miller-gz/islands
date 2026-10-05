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
#pragma once
#include <common.hpp>
#include <network/sessions.hpp>
#include <network/sockets/descriptor.internal.hpp>

// The connecting-state reader shared by the transports whose non-blocking
// connect answers EINPROGRESS instead of completing at once (inet, vsock,
// tls; a unix-domain connect completes or fails immediately): poll(POLLOUT)
// for up to `patience` ms, then getsockopt(SO_ERROR) reads the verdict. A
// patience of 0 only looks, which is what a frame loop may afford; the
// registry's settle() turns repeated looks into a session's standing.

namespace NETWORK {
enum class Dial { PENDING, OPEN, REFUSED };

inline auto resolved(Descriptor descriptor, Integer patience) -> Dial {
  pollfd query{.fd = descriptor, .events = POLLOUT, .revents = 0};
  const auto ready = poll(&query, 1, patience);
  if (ready < 0) return Dial::REFUSED;
  if (ready == 0) return Dial::PENDING;
  int error = 0;
  socklen_t size = sizeof(error);
  getsockopt(
    descriptor, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&error), &size);
  if (error != 0 || (query.revents & (POLLERR | POLLHUP))) return Dial::REFUSED;
  return Dial::OPEN;
}

inline auto settled(Descriptor descriptor) -> Flag {
  return resolved(descriptor, SESSIONS::PATIENCE) == Dial::OPEN;
}
}  // namespace NETWORK
