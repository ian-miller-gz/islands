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
#include <mbedtls/ssl.h>

#include <network/web/native.internal.hpp>

namespace NETWORK::WEB::NATIVE {
namespace {
constexpr Whole PAGE = 16384;
constexpr Integer NOW = 0;
constexpr Integer SHUT = -1;

auto ready(Connection &channel) -> Flag {
  if (channel.ssl && mbedtls_ssl_get_bytes_avail(channel.ssl) > 0) return true;
  pollfd query{.fd = channel.descriptor, .events = POLLIN, .revents = 0};
  return NETWORK::poll(&query, 1, NOW) > 0;
}

auto unsettled(Connection &channel, Integer got) -> Flag {
  if (!channel.ssl) return got < 0 && failing() == AGAIN;
  return got == MBEDTLS_ERR_SSL_WANT_READ ||
         got == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET;
}

auto take(Connection &channel, String &bytes) -> Integer {
  unsigned char page[PAGE];
  const Integer got =
    channel.ssl
      ? mbedtls_ssl_read(channel.ssl, page, PAGE)
      : ::recv(channel.descriptor, reinterpret_cast<Char *>(page), PAGE, 0);
  if (unsettled(channel, got)) return NOW;
  if (got <= 0) return SHUT;
  bytes.append(reinterpret_cast<const char *>(page), static_cast<Whole>(got));
  return got;
}

auto pass(Connection &channel, String &bytes) -> Integer {
  Integer taken = 0;
  while (bytes.size() < CEILING && ready(channel)) {
    const Integer got = take(channel, bytes);
    if (got == SHUT) return SHUT;
    if (got == NOW) return taken;
    taken += got;
  }
  return taken;
}
}  // namespace

auto drain(Connection &channel, String &bytes) -> Integer {
  if (!nonblocking(channel.descriptor)) return SHUT;
  channel.hurried = true;
  const Integer taken = pass(channel, bytes);
  channel.hurried = false;
  return blocking(channel.descriptor) ? taken : SHUT;
}
}  // namespace NETWORK::WEB::NATIVE
