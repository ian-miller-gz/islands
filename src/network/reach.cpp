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
#include <network/reach.hpp>

#if SR_NETWORK_BACKEND != SR_NONE

namespace {
auto enrolled() -> Vector<NETWORK::Protocol> & {
  static Vector<NETWORK::Protocol> protocols;
  return protocols;
}

Flag composed = false;
NETWORK::Transport choice = NETWORK::Transport::UNIX;

auto chosen() -> const NETWORK::Protocol * {
  const NETWORK::Protocol *found = nullptr;
  for (const auto &protocol : enrolled()) {
    if (composed && protocol.transport == choice) return &protocol;
    if (!composed && (!found || protocol.transport < found->transport))
      found = &protocol;
  }
  return found;
}
}  // namespace

auto NETWORK::enroll(const Protocol &protocol) -> Flag {
  enrolled().push_back(protocol);
  return true;
}

auto NETWORK::prefer(Transport transport) -> Flag {
  if (!GET::supported(transport)) return false;
  choice = transport;
  composed = true;
  return true;
}

auto NETWORK::GET::supported(Transport transport) -> Flag {
  for (const auto &protocol : enrolled())
    if (protocol.transport == transport) return true;
  return false;
}

auto NETWORK::GET::preferred() -> Transport {
  const Protocol *protocol = chosen();
  return protocol ? protocol->transport : choice;
}

auto NETWORK::connect(const Wire &wire) -> Reach {
  const Protocol *protocol = chosen();
  return {protocol ? protocol->dial(wire.address) : NONE, wire.address};
}

auto NETWORK::connect(const String &service) -> Reach {
  const Protocol *protocol = chosen();
  if (!protocol) return {};
  return connect(Wire{protocol->resolve(service)});
}

auto NETWORK::listen(const Wire &wire, Whole backlog) -> Reach {
  const Protocol *protocol = chosen();
  return {
    protocol ? protocol->listen(wire.address, backlog) : NONE, wire.address};
}

auto NETWORK::listen(const String &service, Whole backlog) -> Reach {
  const Protocol *protocol = chosen();
  if (!protocol) return {};
  return listen(Wire{protocol->resolve(service)}, backlog);
}

#endif
