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
// A session's standing: DIALING while a non-blocking connect is still out
// (an inet dial answers at once and resolves later), OPEN once the peer
// accepted, CLOSED when it refused, timed out, or hung up. PATIENCE bounds
// a dial, in milliseconds: a connecting-state wait and the deadline after
// which an unanswered dial is given up (Winsock's poll never reports a
// refused loopback connect on older Windows, so the deadline is the only
// verdict there).
enum class State { CLOSED, DIALING, OPEN };
constexpr Integer PATIENCE = 1000;

#if SR_NETWORK_BACKEND != SR_NONE
auto create(const Endpoint &endpoint) -> Handle;
auto create(const Anchor &anchor) -> Handle;
auto create(const Socket &socket) -> Handle;
auto create(const Context &context) -> Handle;
auto create(const Tunnel &tunnel) -> Handle;
auto receive(Handle session, String &data) -> Flag;
auto send(Handle session, const String &data) -> Flag;
auto push(Handle session, const String &data) -> Flag;
// Resolves a dialing session: waits up to `patience` ms for the connect's
// verdict (0 only looks), then answers the session's standing. A refused or
// overdue dial is destroyed. Receiving or sending on a dialing session
// settles it first, so a caller may also just keep polling.
auto settle(Handle session, Integer patience = 0) -> State;
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
inline auto settle(Handle, Integer = 0) -> State { return State::CLOSED; }
inline void destroy(Handle) {}
#endif
}  // namespace NETWORK::SESSIONS

namespace NETWORK::SESSIONS::GET {
#if SR_NETWORK_BACKEND != SR_NONE
auto state(Handle session) -> State;
#else
inline auto state(Handle) -> State { return State::CLOSED; }
#endif

#if SR_NETWORK_CARRIES(SR_UNIX) || SR_NETWORK_CARRIES(SR_ABSTRACT)
auto process(Handle session) -> Whole;
#else
inline auto process(Handle) -> Whole { return 0; }
#endif
}  // namespace NETWORK::SESSIONS::GET
