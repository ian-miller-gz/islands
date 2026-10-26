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
#include <mbedtls/net_sockets.h>

#include <network/web/native.internal.hpp>
#include <network/web/web.internal.hpp>

namespace NETWORK::WEB::NATIVE {
namespace {
int deliver(void *context, const unsigned char *bytes, size_t size) {
  auto *channel = static_cast<Connection *>(context);
  auto wrote = ::send(
    channel->descriptor, reinterpret_cast<const Char *>(bytes), size, QUIET);
  return wrote >= 0 ? static_cast<int>(wrote) : MBEDTLS_ERR_NET_SEND_FAILED;
}

int collect(void *context, unsigned char *bytes, size_t size) {
  auto *channel = static_cast<Connection *>(context);
  auto received =
    ::recv(channel->descriptor, reinterpret_cast<Char *>(bytes), size, 0);
  return received >= 0 ? static_cast<int>(received)
                       : MBEDTLS_ERR_NET_RECV_FAILED;
}

auto handshake(mbedtls_ssl_context *ssl) -> Flag {
  int state = 0;
  while ((state = mbedtls_ssl_handshake(ssl)) != 0)
    if (
      state != MBEDTLS_ERR_SSL_WANT_READ && state != MBEDTLS_ERR_SSL_WANT_WRITE)
      return false;
  return true;
}
}  // namespace

auto secure(Connection &channel, const String &host) -> Flag {
  const mbedtls_ssl_config *role =
    GET::trust(NETWORK::WEB::GET::certificate(host));
  if (!role) return false;
  channel.ssl = new mbedtls_ssl_context;
  mbedtls_ssl_init(channel.ssl);
  if (
    mbedtls_ssl_setup(channel.ssl, role) != 0 ||
    mbedtls_ssl_set_hostname(channel.ssl, host.c_str()) != 0)
    return false;
  mbedtls_ssl_set_bio(channel.ssl, &channel, &deliver, &collect, nullptr);
  return handshake(channel.ssl);
}
}  // namespace NETWORK::WEB::NATIVE
