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
#include <mbedtls/ssl.h>

#include <common.hpp>
#include <network/sockets/descriptor.internal.hpp>
#include <network/web/server.internal.hpp>

namespace NETWORK::WEB::SERVER {

using Moment = long long;
constexpr Moment NEVER = -1;

struct Link {
  Descriptor descriptor = CLOSED;
  mbedtls_ssl_context *ssl = nullptr;
  Flag established = false;
  String pending;
  String outbox;
  Moment opened = NEVER;
  Moment active = NEVER;
};

struct Lane {
  Descriptor listener = CLOSED;
  String wire;
  Handler answering = nullptr;
  const mbedtls_ssl_config *certificate = nullptr;
  Vector<Link *> links;
};

auto lane() -> Lane &;

auto now() -> Moment;

auto listening(const Declared &declared) -> Descriptor;

auto bound(Descriptor listener, const String &address) -> String;

auto certificate(const Declared &declared) -> const mbedtls_ssl_config *;

void shut(Link &link);

auto handshake(Link &link) -> Flag;

auto read(Link &link) -> Flag;

auto flushing(Link &link) -> Flag;

struct Sending {
  Response answer;
  Flag body = true;
};

auto answered(const String &head, Handler answering) -> Sending;

auto composed(const Sending &sending) -> String;

}  // namespace NETWORK::WEB::SERVER
