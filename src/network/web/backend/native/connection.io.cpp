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
}  // namespace

auto send(Connection &channel, const String &bytes) -> Flag {
  for (Whole sent = 0; sent < bytes.size();) {
    const auto *edge = reinterpret_cast<const unsigned char *>(bytes.data());
    auto wrote = channel.ssl ? mbedtls_ssl_write(
                                 channel.ssl, edge + sent, bytes.size() - sent)
                             : ::send(
                                 channel.descriptor, bytes.data() + sent,
                                 bytes.size() - sent, QUIET);
    if (wrote <= 0) return false;
    sent += static_cast<Whole>(wrote);
  }
  return true;
}

auto read(Connection &channel, String &bytes) -> Integer {
  unsigned char page[PAGE];
  auto read =
    channel.ssl
      ? mbedtls_ssl_read(channel.ssl, page, PAGE)
      : ::recv(channel.descriptor, reinterpret_cast<Char *>(page), PAGE, 0);
  if (read == 0 || read == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) return 0;
  if (read < 0) return -1;
  bytes.append(reinterpret_cast<const char *>(page), static_cast<Whole>(read));
  return static_cast<Integer>(read);
}

auto receive(Connection &channel, String &bytes) -> Flag {
  while (bytes.size() < CEILING) {
    Integer got = read(channel, bytes);
    if (got == 0) return true;
    if (got < 0) return false;
  }
  return false;
}
}  // namespace NETWORK::WEB::NATIVE
