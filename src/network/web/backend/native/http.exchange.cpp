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
#include <network/web/native.http.internal.hpp>

namespace NETWORK::WEB::NATIVE::HTTP {

auto exchange(const Request &request, const Url &target) -> Result {
  Connection channel;
  if (!open(target, channel))
    return failed(
      "could not reach " + target.host + ":" + std::to_string(target.port),
      request.url);
  String wire;
  Flag exchanged =
    send(channel, compose(request, target)) && receive(channel, wire);
  close(channel);
  if (!exchanged) return failed("no answer from " + target.host, request.url);
  Result result{};
  if (!interpret(wire, target, result))
    return failed("unreadable answer from " + target.host, request.url);
  return result;
}
}  // namespace NETWORK::WEB::NATIVE::HTTP
