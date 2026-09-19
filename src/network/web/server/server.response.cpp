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
#include <network/web/server.link.internal.hpp>

namespace NETWORK::WEB::SERVER {
namespace {
auto reason(Whole status) -> STRING::Hot {
  switch (status) {
    case 200:
      return "OK";
    case 204:
      return "No Content";
    case 400:
      return "Bad Request";
    case 401:
      return "Unauthorized";
    case 403:
      return "Forbidden";
    case 404:
      return "Not Found";
    case 405:
      return "Method Not Allowed";
    case 413:
      return "Content Too Large";
    case 500:
      return "Internal Server Error";
    default:
      return "Response";
  }
}

auto owed(Whole status) -> String {
  if (status == 401)
    return String("WWW-Authenticate: Basic realm=\"") + REALM +
           "\", charset=\"UTF-8\"\r\n";
  if (status == 405) return "Allow: GET, HEAD\r\n";
  return {};
}
}  // namespace

auto composed(const Sending &sending) -> String {
  const Response &answer = sending.answer;
  String reply = "HTTP/1.1 " + std::to_string(answer.status) + " " +
                 reason(answer.status) + "\r\n";
  reply += "Content-Type: " + answer.type + "\r\n";
  reply += "Content-Length: " + std::to_string(answer.body.size()) + "\r\n";
  reply += "Cache-Control: no-store\r\n";
  reply += "Connection: close\r\n";
  reply += owed(answer.status);
  reply += "\r\n";
  return sending.body ? reply + answer.body : reply;
}

}  // namespace NETWORK::WEB::SERVER
