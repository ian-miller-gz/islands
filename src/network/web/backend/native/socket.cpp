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
#include <algorithm>
#include <network/web/backend.hpp>
#include <network/web/native.socket.internal.hpp>

namespace NETWORK::WEB::NATIVE::SOCKET {
namespace {
Session sessions[WEB::SOCKET::COUNT];
}  // namespace

auto session(WEB::SOCKET::Socket socket) -> Session & {
  return sessions[socket];
}

void release(Session &seat) {
  NATIVE::close(seat.channel);
  seat.channel.pending.clear();
  seat.partial.clear();
}
}  // namespace NETWORK::WEB::NATIVE::SOCKET

namespace NETWORK::WEB::BACKEND::SOCKET {
namespace {
namespace WIRE = NETWORK::WEB::NATIVE::SOCKET;
constexpr STRING::Hot UNREACHED = "socket refused: dial";
constexpr STRING::Hot UNSPOKEN = "socket refused: scheme ";
constexpr STRING::Hot SCHEMES[] = {"ws", "wss"};

auto spoken(const Url &target) -> Flag {
  return std::ranges::find(SCHEMES, target.scheme) != std::end(SCHEMES);
}
}  // namespace

auto open(Socket socket, const String &url, String &error) -> Flag {
  WIRE::Session &seat = WIRE::session(socket);
  WIRE::release(seat);
  const Url target = dissect(url);
  if (!spoken(target)) {
    error = UNSPOKEN + target.scheme;
    return false;
  }
  if (!NATIVE::open(target, seat.channel)) {
    error = UNREACHED;
    return false;
  }
  if (WIRE::upgrade(seat.channel, target, error)) return true;
  WIRE::release(seat);
  return false;
}

auto send(Socket socket, const String &text) -> Flag {
  const String frame = WIRE::encode(WIRE::OPCODE::TEXT, text);
  return !frame.empty() && NATIVE::send(WIRE::session(socket).channel, frame);
}

void close(Socket socket) {
  WIRE::Session &seat = WIRE::session(socket);
  NATIVE::send(seat.channel, WIRE::encode(WIRE::OPCODE::CLOSE, {}));
  WIRE::release(seat);
}
}  // namespace NETWORK::WEB::BACKEND::SOCKET
