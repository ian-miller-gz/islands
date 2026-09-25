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
#include <sys/socket.h>
#include <unistd.h>

#include <network/sockets/sessions.internal.hpp>

auto NETWORK::SESSIONS::GET::process(Handle session) -> Whole {
  auto descriptor = fetch(session);
  if (descriptor == CLOSED) return 0;
  ucred credentials{};
  socklen_t size = sizeof credentials;
  if (getsockopt(descriptor, SOL_SOCKET, SO_PEERCRED, &credentials, &size))
    return 0;
  if (credentials.uid != geteuid()) return 0;
  return credentials.pid > 0 ? static_cast<Whole>(credentials.pid) : 0;
}
