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

#include <network/web/server.link.internal.hpp>

namespace NETWORK::WEB::SERVER {
namespace {
constexpr Whole DRAW = 1024;

int deliver(void *context, const unsigned char *bytes, size_t size) {
  auto *link = static_cast<Link *>(context);
  auto wrote = ::send(
    link->descriptor, reinterpret_cast<const Char *>(bytes), size, QUIET);
  if (wrote >= 0) return static_cast<int>(wrote);
  return failing() == AGAIN ? MBEDTLS_ERR_SSL_WANT_WRITE
                            : MBEDTLS_ERR_NET_SEND_FAILED;
}

int collect(void *context, unsigned char *bytes, size_t size) {
  auto *link = static_cast<Link *>(context);
  auto received =
    ::recv(link->descriptor, reinterpret_cast<Char *>(bytes), size, 0);
  if (received > 0) return static_cast<int>(received);
  if (received == 0) return MBEDTLS_ERR_NET_CONN_RESET;
  return failing() == AGAIN ? MBEDTLS_ERR_SSL_WANT_READ
                            : MBEDTLS_ERR_NET_RECV_FAILED;
}

auto waiting(int state) -> Flag {
  return state == MBEDTLS_ERR_SSL_WANT_READ ||
         state == MBEDTLS_ERR_SSL_WANT_WRITE;
}
}  // namespace

void shut(Link &link) {
  if (link.ssl) {
    mbedtls_ssl_free(link.ssl);
    delete link.ssl;
    link.ssl = nullptr;
  }
  if (link.descriptor != CLOSED) NETWORK::close(link.descriptor);
  link.descriptor = CLOSED;
}

auto handshake(Link &link) -> Flag {
  if (!link.ssl) {
    link.ssl = new mbedtls_ssl_context;
    mbedtls_ssl_init(link.ssl);
    if (mbedtls_ssl_setup(link.ssl, lane().certificate) != 0) return false;
    mbedtls_ssl_set_bio(link.ssl, &link, &deliver, &collect, nullptr);
  }
  const int state = mbedtls_ssl_handshake(link.ssl);
  link.established = state == 0;
  return link.established || waiting(state);
}

auto read(Link &link) -> Flag {
  unsigned char taken[DRAW];
  const int read = mbedtls_ssl_read(link.ssl, taken, sizeof(taken));
  if (read > 0) link.pending.append(reinterpret_cast<Char *>(taken), read);
  return read > 0 || waiting(read);
}

auto flushing(Link &link) -> Flag {
  const int wrote = mbedtls_ssl_write(
    link.ssl, reinterpret_cast<const unsigned char *>(link.outbox.data()),
    link.outbox.size());
  if (wrote > 0) link.outbox.erase(0, static_cast<Whole>(wrote));
  return !link.outbox.empty() && (wrote > 0 || waiting(wrote));
}

}  // namespace NETWORK::WEB::SERVER
