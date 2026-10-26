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

#include <network/web.hpp>
#include <network/web/socket.hpp>

namespace NETWORK::WEB::BACKEND {

auto transfer(const Request &request) -> Result;

}  // namespace NETWORK::WEB::BACKEND

namespace NETWORK::WEB::BACKEND::SOCKET {
using NETWORK::WEB::SOCKET::Socket;
auto open(Socket socket, const String &url, String &error) -> Flag;
auto send(Socket socket, const String &text) -> Flag;
auto receive(Socket socket, Vector<String> &messages) -> Flag;
void close(Socket socket);
}  // namespace NETWORK::WEB::BACKEND::SOCKET
