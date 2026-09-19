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
#pragma once

#include <common.hpp>
#include <network/types.hpp>

namespace NETWORK {

enum class Transport { UNIX, ABSTRACT, INET, VSOCK, TLS };

struct Wire {
  String address;
};

struct Reach {
  Handle handle = NONE;
  String wire;
};

#if SR_NETWORK_BACKEND != SR_NONE
auto connect(const String &service) -> Reach;
auto connect(const Wire &wire) -> Reach;
auto listen(const String &service, Whole backlog = 1) -> Reach;
auto listen(const Wire &wire, Whole backlog = 1) -> Reach;

auto prefer(Transport transport) -> Flag;

namespace GET {
auto preferred() -> Transport;
auto supported(Transport transport) -> Flag;
}  // namespace GET
#else
inline auto connect(const String &) -> Reach { return {}; }
inline auto connect(const Wire &wire) -> Reach { return {NONE, wire.address}; }
inline auto listen(const String &, Whole = 1) -> Reach { return {}; }
inline auto listen(const Wire &wire, Whole = 1) -> Reach {
  return {NONE, wire.address};
}
inline auto prefer(Transport) -> Flag { return false; }

namespace GET {
inline auto preferred() -> Transport { return Transport::UNIX; }
inline auto supported(Transport) -> Flag { return false; }
}  // namespace GET
#endif

struct Protocol {
  Transport transport;
  auto (*dial)(const String &wire) -> Handle;
  auto (*listen)(const String &wire, Whole backlog) -> Handle;
  auto (*resolve)(const String &service) -> String;
};

auto enroll(const Protocol &protocol) -> Flag;

}  // namespace NETWORK
