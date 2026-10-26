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
#include <network/web/backend.hpp>
#include <network/web/backend/selection.hpp>
#include <network/web/socket.hpp>
#include <network/web/web.internal.hpp>

namespace NETWORK::WEB::SOCKET {
namespace {
constexpr STRING::Hot FULL = "socket refused: full";
constexpr STRING::Hot ABSENT = "socket refused: no backend compiled";

Flag standing[COUNT] = {};

auto held(Socket socket) -> Flag { return socket < COUNT && standing[socket]; }

auto vacant() -> Socket {
  for (Socket slot = 0; slot < COUNT; ++slot)
    if (!standing[slot]) return slot;
  return NONE;
}

auto dialed(Socket slot, const String &url, String &error) -> Flag {
#if SR_WEB_BACKEND == SR_NONE
  error = ABSENT;
  return false;
#else
  return BACKEND::SOCKET::open(slot, url, error);
#endif
}
}  // namespace

auto open(const String &url, String &error) -> Socket {
  if (auto denied = screen(url.substr(0, url.find('?')))) {
    error = *denied;
    return NONE;
  }
  const Socket slot = vacant();
  if (slot == NONE) {
    error = FULL;
    return NONE;
  }
  if (!dialed(slot, url, error)) return NONE;
  standing[slot] = true;
  return slot;
}

auto send(Socket socket, const String &text) -> Flag {
#if SR_WEB_BACKEND != SR_NONE
  if (held(socket)) return BACKEND::SOCKET::send(socket, text);
#endif
  return false;
}

auto receive(Socket socket) -> Vector<String> {
  Vector<String> messages;
#if SR_WEB_BACKEND != SR_NONE
  if (held(socket) && !BACKEND::SOCKET::receive(socket, messages))
    standing[socket] = false;
#endif
  return messages;
}

auto connected(Socket socket) -> Flag { return held(socket); }

void close(Socket socket) {
  if (!held(socket)) return;
#if SR_WEB_BACKEND != SR_NONE
  BACKEND::SOCKET::close(socket);
#endif
  standing[socket] = false;
}

void close() {
  for (Socket slot = 0; slot < COUNT; ++slot) close(slot);
}
}  // namespace NETWORK::WEB::SOCKET
