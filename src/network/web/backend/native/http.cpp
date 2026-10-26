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
namespace {
constexpr STRING::Hot BREAK = "\r\n";

auto authority(const Url &target) -> String {
  const Scheme *known = lookup(target.scheme);
  if (known && target.port == known->port) return target.host;
  return target.host + ":" + std::to_string(target.port);
}

auto field(const String &name, const String &value) -> String {
  return name + ": " + value + BREAK;
}
}  // namespace

auto compose(const Request &request, const Url &target) -> String {
  String head = request.method + " " + target.path + " HTTP/1.1" + BREAK;
  head += field("Host", authority(target));
  head += field("User-Agent", ENGINE_NAME);
  head += field("Accept-Encoding", "identity");
  head += field("Connection", "close");
  for (const Header &header : request.headers)
    head += field(header.name, header.value);
  if (!request.body.empty())
    head += field("Content-Length", std::to_string(request.body.size()));
  return head + BREAK + request.body;
}
}  // namespace NETWORK::WEB::NATIVE::HTTP
