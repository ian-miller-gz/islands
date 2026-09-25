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
#include <network/sockets/tls.internal.hpp>

static constexpr Whole CHUNK = 512;

static auto receive(NETWORK::Descriptor descriptor, String &data) -> Flag {
  NETWORK::TLS::Link *link = NETWORK::TLS::find(descriptor);
  if (!link) return false;
  unsigned char chunk[CHUNK];
  while (true) {
    int received = mbedtls_ssl_read(&link->ssl, chunk, sizeof(chunk));
    if (received > 0) {
      data.append(reinterpret_cast<Byte *>(chunk), received);
      continue;
    }
    return received == MBEDTLS_ERR_SSL_WANT_READ ||
           received == MBEDTLS_ERR_SSL_WANT_WRITE;
  }
}

static auto send(NETWORK::Descriptor descriptor, const String &data) -> Flag {
  NETWORK::TLS::Link *link = NETWORK::TLS::find(descriptor);
  if (!link) return false;
  auto *bytes = reinterpret_cast<const unsigned char *>(data.data());
  for (Whole sent = 0; sent < data.size();) {
    int wrote = mbedtls_ssl_write(&link->ssl, bytes + sent, data.size() - sent);
    if (wrote > 0) {
      sent += wrote;
      continue;
    }
    if (
      wrote == MBEDTLS_ERR_SSL_WANT_WRITE && NETWORK::TLS::writable(descriptor))
      continue;
    if (
      wrote == MBEDTLS_ERR_SSL_WANT_READ && NETWORK::TLS::readable(descriptor))
      continue;
    return false;
  }
  return true;
}

static auto push(NETWORK::Descriptor descriptor, const String &data) -> Flag {
  NETWORK::TLS::Link *link = NETWORK::TLS::find(descriptor);
  if (!link) return false;
  auto *bytes = reinterpret_cast<const unsigned char *>(data.data());
  for (Whole sent = 0; sent < data.size();) {
    int wrote = mbedtls_ssl_write(&link->ssl, bytes + sent, data.size() - sent);
    if (wrote <= 0) return false;
    sent += wrote;
  }
  return true;
}

static void shut(NETWORK::Descriptor descriptor) {
  NETWORK::TLS::Link *link = NETWORK::TLS::find(descriptor);
  if (!link) return;
  mbedtls_ssl_close_notify(&link->ssl);
  NETWORK::TLS::discard(link);
}

const NETWORK::SESSIONS::Codec NETWORK::TLS::CODEC = {
  &receive, &send, &push, &shut};
