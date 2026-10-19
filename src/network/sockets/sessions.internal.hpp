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
#include <network.hpp>
#include <network/sockets/descriptor.internal.hpp>

namespace NETWORK::SESSIONS {
struct Codec {
  auto (*receive)(Descriptor descriptor, String &data) -> Flag;
  auto (*send)(Descriptor descriptor, const String &data) -> Flag;
  auto (*push)(Descriptor descriptor, const String &data) -> Flag;
  void (*close)(Descriptor descriptor);
};

inline Vector<Descriptor> descriptors;
inline Vector<const Codec *> codecs;

auto adopt(Descriptor descriptor, const Codec *codec = nullptr) -> Handle;

inline auto fetch(Handle session) -> Descriptor {
  return session < descriptors.size() ? descriptors[session] : CLOSED;
}

inline auto connected(Handle session) -> Flag {
  return fetch(session) != CLOSED;
}
}  // namespace NETWORK::SESSIONS
