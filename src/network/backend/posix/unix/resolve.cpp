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
#include <network/backend/posix/home.internal.hpp>
#include <network/listeners.hpp>
#include <network/protocols.hpp>
#include <network/reach.hpp>
#include <network/sessions.hpp>

auto NETWORK::UNIX::resolve(const String &service) -> Endpoint {
  return {HOME::directory() + "/" + service + ".sock"};
}

auto NETWORK::UNIX::parse(const String &wire) -> Endpoint { return {wire}; }

auto NETWORK::UNIX::format(const Endpoint &endpoint) -> String {
  return endpoint.path;
}

namespace {
auto dialed(const String &wire) -> NETWORK::Handle {
  return NETWORK::SESSIONS::create(NETWORK::UNIX::parse(wire));
}

auto listened(const String &wire, Whole backlog) -> NETWORK::Handle {
  return NETWORK::LISTENERS::create(NETWORK::UNIX::parse(wire), backlog);
}

auto resolved(const String &service) -> String {
  return NETWORK::UNIX::format(NETWORK::UNIX::resolve(service));
}

[[maybe_unused]] const Flag ENROLLED =
  NETWORK::enroll({NETWORK::Transport::UNIX, &dialed, &listened, &resolved});
}  // namespace
