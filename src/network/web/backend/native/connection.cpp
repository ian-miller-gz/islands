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
#include <network/web/native.internal.hpp>

namespace NETWORK::WEB::NATIVE {

auto open(const String &host, Whole port, Connection &channel) -> Flag {
  channel.descriptor = dial(host, port);
  return channel.descriptor != CLOSED;
}

auto open(const Url &target, Connection &channel) -> Flag {
  if (!open(target.host, target.port, channel)) return false;
  const Scheme *known = lookup(target.scheme);
  if (!known || !known->secured) return true;
  if (secure(channel, target.host)) return true;
  close(channel);
  return false;
}

void close(Connection &channel) {
  if (channel.ssl) {
    mbedtls_ssl_close_notify(channel.ssl);
    mbedtls_ssl_free(channel.ssl);
    delete channel.ssl;
    channel.ssl = nullptr;
  }
  if (channel.descriptor != CLOSED) {
    shutdown(channel.descriptor, DUPLEX);
    NETWORK::close(channel.descriptor);
    channel.descriptor = CLOSED;
  }
}
}  // namespace NETWORK::WEB::NATIVE
