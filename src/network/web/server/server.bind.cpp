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
#include <network/web/server.link.internal.hpp>
#define LOGGER_CATEGORY "~/network::serve"

namespace NETWORK::WEB::SERVER {
auto listening(const Declared &declared) -> Descriptor {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  if (!startup()) return CLOSED;
  sockaddr_in6 six{};
  sockaddr_in four{};
  void *seat = nullptr;
  socklen_t size = 0;
  int family = AF_INET;
  if (inet_pton(AF_INET, declared.address.c_str(), &four.sin_addr) == 1) {
    four.sin_family = AF_INET;
    four.sin_port = htons(static_cast<uint16_t>(declared.port));
    seat = &four;
    size = sizeof(four);
  } else if (
    inet_pton(AF_INET6, declared.address.c_str(), &six.sin6_addr) == 1) {
    six.sin6_family = family = AF_INET6;
    six.sin6_port = htons(static_cast<uint16_t>(declared.port));
    seat = &six;
    size = sizeof(six);
  } else {
    logger.error("Serving refused: " + declared.address + " is not an address");
    return CLOSED;
  }
  Descriptor listener = opened(family, SOCK_STREAM, 0);
  if (listener == CLOSED) return listener;
  int reuse = 1;
  setsockopt(
    listener, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&reuse),
    sizeof(reuse));
  if (
    ::bind(listener, static_cast<sockaddr *>(seat), size) != 0 ||
    listen(listener, static_cast<int>(LINKS)) != 0) {
    logger.error(
      "Serving refused: cannot bind " + declared.address + ":" +
      std::to_string(declared.port));
    NETWORK::close(listener);
    return CLOSED;
  }
  return listener;
}

auto bound(Descriptor listener, const String &address) -> String {
  sockaddr_in6 seat{};
  socklen_t size = sizeof(seat);
  Whole port = 0;
  if (getsockname(listener, reinterpret_cast<sockaddr *>(&seat), &size) == 0)
    port = ntohs(
      seat.sin6_family == AF_INET6
        ? seat.sin6_port
        : reinterpret_cast<sockaddr_in *>(&seat)->sin_port);
  return address + ":" + std::to_string(port);
}
}  // namespace NETWORK::WEB::SERVER
