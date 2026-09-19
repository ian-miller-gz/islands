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
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <common.hpp>

namespace NETWORK {
using Descriptor = int;
constexpr Descriptor CLOSED = -1;
constexpr Integer QUIET = MSG_NOSIGNAL;
constexpr Integer HURRIED = MSG_NOSIGNAL | MSG_DONTWAIT;
constexpr Integer PENDING = EINPROGRESS;
constexpr Integer AGAIN = EAGAIN;
constexpr Integer INTERRUPTED = EINTR;
constexpr Integer DUPLEX = SHUT_RDWR;

inline auto startup() -> Flag { return true; }

inline auto failing() -> Integer { return errno; }
inline auto failure() -> String { return std::strerror(errno); }

inline auto opened(Integer family, Integer kind, Integer protocol)
  -> Descriptor {
  return ::socket(family, kind | SOCK_NONBLOCK | SOCK_CLOEXEC, protocol);
}

inline auto accepted(Descriptor listener) -> Descriptor {
  return ::accept4(listener, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
}

inline auto nonblocking(Descriptor descriptor) -> Flag {
  return ::fcntl(descriptor, F_SETFL, O_NONBLOCK) == 0;
}

inline auto blocking(Descriptor descriptor) -> Flag {
  return ::fcntl(descriptor, F_SETFL, 0) == 0;
}

inline auto timeout(Descriptor descriptor, Whole milliseconds) -> Flag {
  timeval limit{
    .tv_sec = static_cast<time_t>(milliseconds / 1000),
    .tv_usec = static_cast<suseconds_t>(milliseconds % 1000 * 1000)};
  const auto *seat = reinterpret_cast<const char *>(&limit);
  return ::setsockopt(
           descriptor, SOL_SOCKET, SO_RCVTIMEO, seat, sizeof(limit)) == 0 &&
         ::setsockopt(
           descriptor, SOL_SOCKET, SO_SNDTIMEO, seat, sizeof(limit)) == 0;
}

inline auto poll(pollfd *queries, Whole count, Integer patience) -> Integer {
  return ::poll(queries, count, static_cast<int>(patience));
}

inline void close(Descriptor descriptor) { ::close(descriptor); }
}  // namespace NETWORK
