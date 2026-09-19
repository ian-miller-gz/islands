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
#include <cstdlib>
#include <cstring>
#include <network/backend/posix/inet.internal.hpp>
#include <network/backend/posix/service.internal.hpp>
#include <network/listeners.hpp>
#include <network/protocols.hpp>
#include <network/reach.hpp>
#include <network/sessions.hpp>

auto NETWORK::TLS::resolve(const String &service) -> Tunnel {
  return {INET::LOOPBACK, SERVICE::port(service)};
}

auto NETWORK::TLS::parse(const String &wire) -> Tunnel {
  const auto open = std::strlen(INET::OPEN);
  const auto shut = wire.rfind(INET::SHUT);
  if (shut == String::npos || wire.compare(0, open, INET::OPEN) != 0)
    return {wire, 0};
  return {
    wire.substr(open, shut - open),
    std::strtoul(wire.c_str() + shut + std::strlen(INET::SHUT), nullptr, 10)};
}

auto NETWORK::TLS::format(const Tunnel &tunnel) -> String {
  return INET::OPEN + tunnel.host + INET::SHUT + std::to_string(tunnel.port);
}

namespace {
auto dialed(const String &wire) -> NETWORK::Handle {
  return NETWORK::SESSIONS::create(NETWORK::TLS::parse(wire));
}

auto listened(const String &wire, Whole backlog) -> NETWORK::Handle {
  return NETWORK::LISTENERS::create(NETWORK::TLS::parse(wire), backlog);
}

auto resolved(const String &service) -> String {
  return NETWORK::TLS::format(NETWORK::TLS::resolve(service));
}

[[maybe_unused]] const Flag ENROLLED =
  NETWORK::enroll({NETWORK::Transport::TLS, &dialed, &listened, &resolved});
}  // namespace
