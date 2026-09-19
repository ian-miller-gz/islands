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
#include <cstdint>
#include <type_traits>
#include <common.hpp>
#include <network/backend/posix/descriptor.internal.hpp>
#include <network/backend/selection.hpp>
#include <network/types.hpp>

namespace NETWORK::INET {
struct Flavour {
  Whole token;
  int family;
  int protocol;
  STRING::Hot loopback;
  STRING::Hot open;
  STRING::Hot shut;
};

constexpr Flavour FLAVOURS[]{
  {SR_TCP, AF_INET, 0, "127.0.0.1", "", ":"},
  {SR_TCP6, AF_INET6, 0, "::1", "[", "]:"},
  {SR_SCTP, AF_INET, IPPROTO_SCTP, "127.0.0.1", "", ":"},
};

consteval auto choose() -> Flavour {
  for (const Flavour &row : FLAVOURS)
    if (SR_NETWORK_CARRIES(row.token)) return row;
  return FLAVOURS[0];
}

constexpr Flavour SELECTED = choose();
constexpr int FAMILY = SELECTED.family;
constexpr int PROTOCOL = SELECTED.protocol;
constexpr STRING::Hot LOOPBACK = SELECTED.loopback;
constexpr STRING::Hot OPEN = SELECTED.open;
constexpr STRING::Hot SHUT = SELECTED.shut;

using Sockaddr =
  std::conditional_t<FAMILY == AF_INET6, sockaddr_in6, sockaddr_in>;

inline auto resolve(const Socket &socket, sockaddr_in &address) -> Flag {
  address.sin_family = AF_INET;
  address.sin_port = htons(static_cast<uint16_t>(socket.port));
  return inet_pton(AF_INET, socket.host.c_str(), &address.sin_addr) == 1;
}

inline auto resolve(const Socket &socket, sockaddr_in6 &address) -> Flag {
  address.sin6_family = AF_INET6;
  address.sin6_port = htons(static_cast<uint16_t>(socket.port));
  return inet_pton(AF_INET6, socket.host.c_str(), &address.sin6_addr) == 1;
}
}  // namespace NETWORK::INET
