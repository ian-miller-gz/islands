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
#ifndef NOUSER
#define NOUSER
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#include <common.hpp>
#include <string>

namespace NETWORK {
using Descriptor = SOCKET;
constexpr Descriptor CLOSED = INVALID_SOCKET;
constexpr Integer QUIET = 0;
constexpr Integer HURRIED = 0;
constexpr Integer PENDING = WSAEWOULDBLOCK;
constexpr Integer AGAIN = WSAEWOULDBLOCK;
constexpr Integer INTERRUPTED = WSAEINTR;
constexpr Integer DUPLEX = SD_BOTH;

inline auto failing() -> Integer { return ::WSAGetLastError(); }
inline auto failure() -> String { return std::to_string(::WSAGetLastError()); }

inline auto startup() -> Flag {
  static const Flag ready = [] {
    WSADATA data;
    return ::WSAStartup(MAKEWORD(2, 2), &data) == 0;
  }();
  return ready;
}

inline auto nonblocking(Descriptor descriptor) -> Flag {
  u_long wanted = 1;
  return ::ioctlsocket(descriptor, FIONBIO, &wanted) == 0;
}

inline auto blocking(Descriptor descriptor) -> Flag {
  u_long wanted = 0;
  return ::ioctlsocket(descriptor, FIONBIO, &wanted) == 0;
}

inline void close(Descriptor descriptor) { ::closesocket(descriptor); }

inline auto unblocked(Descriptor descriptor) -> Descriptor {
  if (descriptor == CLOSED || nonblocking(descriptor)) return descriptor;
  close(descriptor);
  return CLOSED;
}

inline auto opened(Integer family, Integer kind, Integer protocol)
  -> Descriptor {
  if (!startup()) return CLOSED;
  return unblocked(::WSASocketW(
    family, kind, protocol, nullptr, 0, WSA_FLAG_NO_HANDLE_INHERIT));
}

inline auto accepted(Descriptor listener) -> Descriptor {
  return unblocked(::accept(listener, nullptr, nullptr));
}

inline auto timeout(Descriptor descriptor, Whole milliseconds) -> Flag {
  DWORD limit = static_cast<DWORD>(milliseconds);
  const auto *seat = reinterpret_cast<const char *>(&limit);
  return ::setsockopt(
           descriptor, SOL_SOCKET, SO_RCVTIMEO, seat, sizeof(limit)) == 0 &&
         ::setsockopt(
           descriptor, SOL_SOCKET, SO_SNDTIMEO, seat, sizeof(limit)) == 0;
}

inline auto poll(pollfd *queries, Whole count, Integer patience) -> Integer {
  return ::WSAPoll(
    queries, static_cast<ULONG>(count), static_cast<INT>(patience));
}
}  // namespace NETWORK
