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

namespace NETWORK::LISTENERS {
#if SR_NETWORK_BACKEND != SR_NONE
auto create(const Endpoint &endpoint, Whole backlog = 1) -> Handle;
auto create(const Anchor &anchor, Whole backlog = 1) -> Handle;
auto create(const Socket &socket, Whole backlog = 1) -> Handle;
auto create(const Context &context, Whole backlog = 1) -> Handle;
auto create(const Tunnel &tunnel, Whole backlog = 1) -> Handle;
auto accept(Handle listener) -> Handle;
void destroy(Handle listener);
#else
inline auto create(const Endpoint &, Whole = 1) -> Handle { return NONE; }
inline auto create(const Anchor &, Whole = 1) -> Handle { return NONE; }
inline auto create(const Socket &, Whole = 1) -> Handle { return NONE; }
inline auto create(const Context &, Whole = 1) -> Handle { return NONE; }
inline auto create(const Tunnel &, Whole = 1) -> Handle { return NONE; }
inline auto accept(Handle) -> Handle { return NONE; }
inline void destroy(Handle) {}
#endif
}  // namespace NETWORK::LISTENERS
