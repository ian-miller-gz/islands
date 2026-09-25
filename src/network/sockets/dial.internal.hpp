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
#include <network/sockets/descriptor.internal.hpp>

namespace NETWORK {
constexpr Integer PATIENCE = 1000;

inline auto settled(Descriptor descriptor) -> Flag {
  pollfd query{.fd = descriptor, .events = POLLOUT, .revents = 0};
  if (poll(&query, 1, PATIENCE) <= 0) return false;
  int error = 0;
  socklen_t size = sizeof(error);
  getsockopt(
    descriptor, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&error), &size);
  return error == 0;
}
}  // namespace NETWORK
