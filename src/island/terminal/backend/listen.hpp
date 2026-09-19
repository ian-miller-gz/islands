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

#include <island/terminal/backend/selection.hpp>
#include <common.hpp>
#include <network.hpp>
#include <optional>

namespace TERMINAL::BACKEND::LISTEN {
#if SR_LISTEN == SR_UNIX
inline Flag on = true;
inline std::optional<NETWORK::Wire> self;
inline std::optional<NETWORK::Wire> roster;
void initialize();
void process();
void close();
#else
inline void initialize() {}
inline void process() {}
inline void close() {}
#endif
}  // namespace TERMINAL::BACKEND::LISTEN
