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
#include <network/backend/posix/service.internal.hpp>
#include <network/backend/posix/vsock.internal.hpp>
#include <network/listeners.hpp>
#include <network/protocols.hpp>
#include <network/reach.hpp>
#include <network/sessions.hpp>

auto NETWORK::VSOCK::resolve(const String &service) -> Context {
  return {LOCAL, SERVICE::port(service)};
}

auto NETWORK::VSOCK::parse(const String &wire) -> Context {
  const auto shut = wire.rfind(':');
  if (shut == String::npos) return {LOCAL, 0};
  return {
    std::strtoul(wire.c_str(), nullptr, 10),
    std::strtoul(wire.c_str() + shut + 1, nullptr, 10)};
}

auto NETWORK::VSOCK::format(const Context &context) -> String {
  return std::to_string(context.cid) + ":" + std::to_string(context.port);
}

namespace {
auto dialed(const String &wire) -> NETWORK::Handle {
  return NETWORK::SESSIONS::create(NETWORK::VSOCK::parse(wire));
}

auto listened(const String &wire, Whole backlog) -> NETWORK::Handle {
  return NETWORK::LISTENERS::create(NETWORK::VSOCK::parse(wire), backlog);
}

auto resolved(const String &service) -> String {
  return NETWORK::VSOCK::format(NETWORK::VSOCK::resolve(service));
}

[[maybe_unused]] const Flag ENROLLED =
  NETWORK::enroll({NETWORK::Transport::VSOCK, &dialed, &listened, &resolved});
}  // namespace
