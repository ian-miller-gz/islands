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
#include <network/web/native.internal.hpp>

namespace NETWORK::WEB::NATIVE {
namespace {
auto connected(Descriptor descriptor) -> Flag {
  pollfd query{.fd = descriptor, .events = POLLOUT, .revents = 0};
  if (NETWORK::poll(&query, 1, PATIENCE) <= 0) return false;
  int error = 0;
  socklen_t size = sizeof(error);
  getsockopt(
    descriptor, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&error), &size);
  return error == 0;
}

auto reach(const addrinfo &candidate) -> Descriptor {
  Descriptor descriptor =
    opened(candidate.ai_family, candidate.ai_socktype, candidate.ai_protocol);
  if (descriptor == CLOSED) return CLOSED;
  Flag settled = connect(
                   descriptor, candidate.ai_addr,
                   static_cast<socklen_t>(candidate.ai_addrlen)) == 0 ||
                 (failing() == PENDING && connected(descriptor));
  if (settled && blocking(descriptor) && timeout(descriptor, PATIENCE))
    return descriptor;
  NETWORK::close(descriptor);
  return CLOSED;
}
}  // namespace

auto dial(const String &host, Whole port) -> Descriptor {
  if (!startup()) return CLOSED;
  addrinfo wanted{};
  wanted.ai_family = AF_UNSPEC;
  wanted.ai_socktype = SOCK_STREAM;
  addrinfo *answers = nullptr;
  if (
    getaddrinfo(
      host.c_str(), std::to_string(port).c_str(), &wanted, &answers) != 0)
    return CLOSED;
  Descriptor descriptor = CLOSED;
  for (const addrinfo *candidate = answers; candidate && descriptor == CLOSED;
       candidate = candidate->ai_next)
    descriptor = reach(*candidate);
  freeaddrinfo(answers);
  return descriptor;
}
}  // namespace NETWORK::WEB::NATIVE
