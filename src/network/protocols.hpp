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
#include <network/types.hpp>

namespace NETWORK {

namespace UNIX {
auto resolve(const String &service) -> Endpoint;
auto parse(const String &wire) -> Endpoint;
auto format(const Endpoint &endpoint) -> String;
}  // namespace UNIX

namespace ABSTRACT {
auto resolve(const String &service) -> Anchor;
auto parse(const String &wire) -> Anchor;
auto format(const Anchor &anchor) -> String;
}  // namespace ABSTRACT

namespace INET {
auto resolve(const String &service) -> Socket;
auto parse(const String &wire) -> Socket;
auto format(const Socket &socket) -> String;
}  // namespace INET

namespace VSOCK {
auto resolve(const String &service) -> Context;
auto parse(const String &wire) -> Context;
auto format(const Context &context) -> String;
}  // namespace VSOCK

namespace TLS {
auto resolve(const String &service) -> Tunnel;
auto parse(const String &wire) -> Tunnel;
auto format(const Tunnel &tunnel) -> String;
}  // namespace TLS

}  // namespace NETWORK
