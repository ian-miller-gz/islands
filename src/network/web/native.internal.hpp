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
#include <network/backend/posix/descriptor.internal.hpp>
#include <network/web.hpp>
#include <network/web/url.internal.hpp>

namespace NETWORK::WEB::NATIVE {
constexpr Integer PATIENCE = 10000;
constexpr Whole CEILING = 32u << 20;

struct Connection {
  Descriptor descriptor = CLOSED;
  mbedtls_ssl_context *ssl = nullptr;
  String pending;
};

auto dial(const String &host, Whole port) -> Descriptor;
auto secure(Connection &channel, const String &host) -> Flag;
auto open(const String &host, Whole port, Connection &channel) -> Flag;
auto open(const Url &target, Connection &channel) -> Flag;
void close(Connection &channel);

auto send(Connection &channel, const String &bytes) -> Flag;
auto read(Connection &channel, String &bytes) -> Integer;
auto receive(Connection &channel, String &bytes) -> Flag;

auto failed(const String &reason, const String &url) -> Result;

auto shape(mbedtls_ssl_config &config, mbedtls_x509_crt &roots) -> Flag;

namespace GET {
auto trust() -> const mbedtls_ssl_config *;
auto trust(const String &certificate) -> const mbedtls_ssl_config *;
}  // namespace GET
}  // namespace NETWORK::WEB::NATIVE
