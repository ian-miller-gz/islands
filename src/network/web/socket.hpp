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

namespace NETWORK::WEB::SOCKET {
using Socket = Whole;
constexpr Socket NONE = ~Whole{0};
constexpr Whole COUNT = 4;
constexpr Whole CEILING = 1u << 20;
auto open(const String &url, String &error) -> Socket;
auto send(Socket socket, const String &text) -> Flag;
auto receive(Socket socket) -> Vector<String>;
auto connected(Socket socket) -> Flag;
void close(Socket socket);
void close();
}  // namespace NETWORK::WEB::SOCKET
