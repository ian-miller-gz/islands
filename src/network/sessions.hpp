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

namespace NETWORK::SESSIONS {
#if SR_NETWORK_BACKEND != SR_NONE
auto create(const Endpoint &endpoint) -> Handle;
auto create(const Anchor &anchor) -> Handle;
auto create(const Socket &socket) -> Handle;
auto create(const Context &context) -> Handle;
auto create(const Tunnel &tunnel) -> Handle;
auto receive(Handle session, String &data) -> Flag;
auto send(Handle session, const String &data) -> Flag;
auto push(Handle session, const String &data) -> Flag;
void destroy(Handle session);
#else
inline auto create(const Endpoint &) -> Handle { return NONE; }
inline auto create(const Anchor &) -> Handle { return NONE; }
inline auto create(const Socket &) -> Handle { return NONE; }
inline auto create(const Context &) -> Handle { return NONE; }
inline auto create(const Tunnel &) -> Handle { return NONE; }
inline auto receive(Handle, String &) -> Flag { return false; }
inline auto send(Handle, const String &) -> Flag { return false; }
inline auto push(Handle, const String &) -> Flag { return false; }
inline void destroy(Handle) {}
#endif
}  // namespace NETWORK::SESSIONS

namespace NETWORK::SESSIONS::GET {
#if SR_NETWORK_CARRIES(SR_UNIX) || SR_NETWORK_CARRIES(SR_ABSTRACT)
auto process(Handle session) -> Whole;
#else
inline auto process(Handle) -> Whole { return 0; }
#endif
}  // namespace NETWORK::SESSIONS::GET
