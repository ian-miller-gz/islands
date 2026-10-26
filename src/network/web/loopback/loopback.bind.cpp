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
#include <chrono>
#include <logger.hpp>
#include <network/web/loopback.link.internal.hpp>
#define LOGGER_CATEGORY "~/network::loopback"

namespace NETWORK::WEB::LOOPBACK {
namespace {
constexpr Integer BACKLOG = static_cast<Integer>(LINKS);
constexpr uint16_t EPHEMERAL = 0;

auto listening() -> Descriptor {
  sockaddr_in seat{};
  seat.sin_family = AF_INET;
  seat.sin_port = htons(EPHEMERAL);
  if (!startup() || inet_pton(AF_INET, ADDRESS, &seat.sin_addr) != 1)
    return CLOSED;
  Descriptor listener = opened(AF_INET, SOCK_STREAM, 0);
  if (listener == CLOSED) return listener;
  if (
    ::bind(listener, reinterpret_cast<sockaddr *>(&seat), sizeof(seat)) != 0 ||
    listen(listener, BACKLOG) != 0) {
    NETWORK::close(listener);
    return CLOSED;
  }
  return listener;
}

auto port(Descriptor listener) -> Whole {
  sockaddr_in seat{};
  socklen_t size = sizeof(seat);
  if (getsockname(listener, reinterpret_cast<sockaddr *>(&seat), &size) != 0)
    return 0;
  return ntohs(seat.sin_port);
}
}  // namespace

auto lane() -> Lane & {
  static Lane one;
  return one;
}

auto now() -> Moment {
  using std::chrono::steady_clock;
  return std::chrono::duration_cast<std::chrono::milliseconds>(
           steady_clock::now().time_since_epoch())
    .count();
}

auto bind(const String &path, Answering answering) -> Whole {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  Lane &one = lane();
  one.listener = listening();
  const Whole bound = one.listener == CLOSED ? 0 : port(one.listener);
  if (bound == 0) {
    logger.error(String("Loopback refused: cannot bind ") + ADDRESS);
    release();
    return 0;
  }
  one.path = path;
  one.answering = answering;
  return bound;
}

void release() {
  Lane &one = lane();
  for (Link &link : one.links) NETWORK::close(link.descriptor);
  one.links.clear();
  if (one.listener != CLOSED) NETWORK::close(one.listener);
  one = {};
}
}  // namespace NETWORK::WEB::LOOPBACK
