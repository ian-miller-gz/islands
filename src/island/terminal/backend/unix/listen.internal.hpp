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
#include <island/terminal/backend/listen.hpp>
#include <shell/engine/engine.hpp>
#include <common.hpp>
#include <network.hpp>

namespace TERMINAL::BACKEND::LISTEN {
inline NETWORK::Handle listener = NETWORK::NONE;
inline NETWORK::Handle session = NETWORK::NONE;
inline NETWORK::Handle daemon = NETWORK::NONE;
inline NETWORK::Handle broker = NETWORK::NONE;
inline Flag streaming = false;
void send(const String &text);
void serve();
void drop();
void announce();
void withdraw();
void join();
void publish();
void leave();
auto intercept(SHELL::Session &session, const String &line) -> Flag;
void stream();
}  // namespace TERMINAL::BACKEND::LISTEN
