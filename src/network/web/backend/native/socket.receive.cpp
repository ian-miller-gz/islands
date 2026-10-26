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
#include <network/web/native.socket.internal.hpp>

namespace NETWORK::WEB::NATIVE::SOCKET {
namespace {
using Answering = auto (*)(Session &, const Frame &, Vector<String> &) -> Flag;

struct Answer {
  Octet opcode;
  Answering answering;
};

auto joined(Session &seat, const Frame &frame, Vector<String> &messages)
  -> Flag {
  if (seat.partial.size() + frame.payload.size() > WEB::SOCKET::CEILING)
    return false;
  seat.partial += frame.payload;
  if (!frame.fin) return true;
  messages.push_back(seat.partial);
  seat.partial.clear();
  return true;
}

auto text(Session &seat, const Frame &frame, Vector<String> &messages) -> Flag {
  seat.partial.clear();
  return joined(seat, frame, messages);
}

auto pinged(Session &seat, const Frame &frame, Vector<String> &) -> Flag {
  return send(seat.channel, encode(OPCODE::PONG, frame.payload));
}

auto ponged(Session &, const Frame &, Vector<String> &) -> Flag { return true; }

auto closed(Session &seat, const Frame &, Vector<String> &) -> Flag {
  send(seat.channel, encode(OPCODE::CLOSE, {}));
  return false;
}

constexpr Answer ANSWERS[] = {
  {OPCODE::CONTINUATION, joined}, {OPCODE::TEXT, text},
  {OPCODE::CLOSE, closed},        {OPCODE::PING, pinged},
  {OPCODE::PONG, ponged},
};

auto answered(Session &seat, const Frame &frame, Vector<String> &messages)
  -> Flag {
  for (const Answer &row : ANSWERS)
    if (row.opcode == frame.opcode) return row.answering(seat, frame, messages);
  return false;
}

auto heard(Session &seat, Vector<String> &messages) -> Flag {
  Frame frame;
  Integer step = 0;
  while ((step = decode(seat.channel.pending, frame)) > 0)
    if (!answered(seat, frame, messages)) return false;
  return step == 0;
}
}  // namespace
}  // namespace NETWORK::WEB::NATIVE::SOCKET

auto NETWORK::WEB::BACKEND::SOCKET::receive(
  Socket socket, Vector<String> &messages) -> Flag {
  namespace WIRE = NETWORK::WEB::NATIVE::SOCKET;
  WIRE::Session &seat = WIRE::session(socket);
  const Flag open = NATIVE::drain(seat.channel, seat.channel.pending) >= 0;
  if (WIRE::heard(seat, messages) && open) return true;
  WIRE::release(seat);
  return false;
}
