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
#include <network/backend/posix/tls.internal.hpp>

#include <mbedtls/net_sockets.h>

static int deliver(void *context, const unsigned char *bytes, size_t size) {
  auto *link = static_cast<NETWORK::TLS::Link *>(context);
  auto wrote = ::send(
    link->descriptor, reinterpret_cast<STRING::Hot>(bytes),
    static_cast<int>(size), NETWORK::QUIET);
  if (wrote >= 0) return static_cast<int>(wrote);
  const auto trouble = NETWORK::failing();
  if (trouble == NETWORK::AGAIN || trouble == NETWORK::INTERRUPTED)
    return MBEDTLS_ERR_SSL_WANT_WRITE;
  return MBEDTLS_ERR_NET_SEND_FAILED;
}

static int collect(void *context, unsigned char *bytes, size_t size) {
  auto *link = static_cast<NETWORK::TLS::Link *>(context);
  auto received = ::recv(
    link->descriptor, reinterpret_cast<Byte *>(bytes), static_cast<int>(size),
    0);
  if (received >= 0) return static_cast<int>(received);
  const auto trouble = NETWORK::failing();
  if (trouble == NETWORK::AGAIN || trouble == NETWORK::INTERRUPTED)
    return MBEDTLS_ERR_SSL_WANT_READ;
  return MBEDTLS_ERR_NET_RECV_FAILED;
}

auto NETWORK::TLS::find(Descriptor descriptor) -> Link * {
  for (Link *link : links)
    if (link && link->descriptor == descriptor) return link;
  return nullptr;
}

auto NETWORK::TLS::secure(Descriptor descriptor, const mbedtls_ssl_config *role)
  -> Link * {
  if (!role) return nullptr;
  auto *link = new Link{descriptor};
  mbedtls_ssl_init(&link->ssl);
  if (mbedtls_ssl_setup(&link->ssl, role) != 0) {
    mbedtls_ssl_free(&link->ssl);
    delete link;
    return nullptr;
  }
  mbedtls_ssl_set_bio(&link->ssl, link, &deliver, &collect, nullptr);
  for (Whole slot = 0; slot < links.size(); ++slot)
    if (!links[slot]) return links[slot] = link;
  links.push_back(link);
  return link;
}

void NETWORK::TLS::discard(Link *link) {
  for (Whole slot = 0; slot < links.size(); ++slot)
    if (links[slot] == link) links[slot] = nullptr;
  mbedtls_ssl_free(&link->ssl);
  delete link;
}

auto NETWORK::TLS::shaken(Link *link) -> Flag {
  if (!link) return false;
  while (true) {
    int state = mbedtls_ssl_handshake(&link->ssl);
    if (state == 0) return true;
    if (state == MBEDTLS_ERR_SSL_WANT_READ && readable(link->descriptor))
      continue;
    if (state == MBEDTLS_ERR_SSL_WANT_WRITE && writable(link->descriptor))
      continue;
    return false;
  }
}
