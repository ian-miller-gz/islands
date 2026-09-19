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
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ssl.h>

#include <common.hpp>
#include <network/backend/posix/dial.internal.hpp>
#include <network/backend/posix/sessions.internal.hpp>

namespace NETWORK::TLS {
constexpr STRING::Hot PEER = "islands";

struct Link {
  Descriptor descriptor = CLOSED;
  mbedtls_ssl_context ssl;
};
inline Vector<Link *> links;

auto find(Descriptor descriptor) -> Link *;
auto secure(Descriptor descriptor, const mbedtls_ssl_config *role) -> Link *;
void discard(Link *link);
auto shaken(Link *link) -> Flag;
auto identify() -> Flag;
auto generate() -> Flag;

extern const SESSIONS::Codec CODEC;

namespace GET {
auto generator() -> mbedtls_ctr_drbg_context *;
auto client() -> const mbedtls_ssl_config *;
auto server() -> const mbedtls_ssl_config *;
auto anchor() -> String;
auto secret() -> String;
}  // namespace GET

inline auto readable(Descriptor descriptor) -> Flag {
  pollfd query{.fd = descriptor, .events = POLLIN, .revents = 0};
  return NETWORK::poll(&query, 1, PATIENCE) > 0;
}

inline auto writable(Descriptor descriptor) -> Flag {
  pollfd query{.fd = descriptor, .events = POLLOUT, .revents = 0};
  return NETWORK::poll(&query, 1, PATIENCE) > 0;
}
}  // namespace NETWORK::TLS
