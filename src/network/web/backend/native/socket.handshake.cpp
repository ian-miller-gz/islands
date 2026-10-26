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
#include <cctype>
#include <cstring>
#include <network/web/native.http.internal.hpp>
#include <network/web/native.socket.internal.hpp>

namespace NETWORK::WEB::NATIVE::SOCKET {
namespace {
constexpr STRING::Hot BREAK = "\r\n";
constexpr STRING::Hot END = "\r\n\r\n";
constexpr STRING::Hot ACCEPT = "\r\nsec-websocket-accept:";
constexpr STRING::Hot VERSION = "13";
constexpr STRING::Hot SILENT = "socket refused: silent";
constexpr STRING::Hot UNSENT = "socket refused: upgrade";
constexpr STRING::Hot FORGED = "socket refused: accept";
constexpr STRING::Hot STATUS = "socket refused: status ";
constexpr Whole SWITCHING = 101;
constexpr Whole DIGITS = 3;
constexpr Whole HEAD = 16384;

auto request(const Url &target, const String &key) -> String {
  return "GET " + target.path +
         " HTTP/1.1\r\nHost: " + HTTP::authority(target) +
         "\r\nUpgrade: websocket\r\nConnection: "
         "Upgrade\r\nSec-WebSocket-Key: " +
         key + "\r\nSec-WebSocket-Version: " + VERSION + END;
}

auto head(Connection &channel, String &bytes) -> Whole {
  while (bytes.size() < HEAD) {
    const Integer got = drain(channel, bytes);
    if (auto end = bytes.find(END); end != String::npos) return end;
    if (got < 0) return String::npos;
    if (got > 0) continue;
    pollfd query{.fd = channel.descriptor, .events = POLLIN, .revents = 0};
    if (NETWORK::poll(&query, 1, PATIENCE) <= 0) return String::npos;
  }
  return String::npos;
}

auto stated(const String &answer) -> Whole {
  const auto space = answer.find(' ');
  if (!answer.starts_with("HTTP/") || space == String::npos) return 0;
  return number(answer.substr(space + 1, DIGITS));
}

auto accepted(const String &answer) -> String {
  String folded = answer;
  for (Char &letter : folded)
    letter =
      static_cast<Char>(std::tolower(static_cast<unsigned char>(letter)));
  const auto at = folded.find(ACCEPT);
  if (at == String::npos) return {};
  const auto from = answer.find_first_not_of(' ', at + std::strlen(ACCEPT));
  if (from == String::npos) return {};
  return answer.substr(from, answer.find(BREAK, from) - from);
}

auto refused(String &error, const String &reason) -> Flag {
  error = reason;
  return false;
}
}  // namespace

auto upgrade(Connection &channel, const Url &target, String &error) -> Flag {
  const String key = nonce();
  if (key.empty() || !send(channel, request(target, key)))
    return refused(error, UNSENT);
  const Whole end = head(channel, channel.pending);
  if (end == String::npos) return refused(error, SILENT);
  const String answer = channel.pending.substr(0, end);
  channel.pending.erase(0, end + std::strlen(END));
  if (const Whole status = stated(answer); status != SWITCHING)
    return refused(error, STATUS + std::to_string(status));
  if (accepted(answer) != expected(key)) return refused(error, FORGED);
  return true;
}
}  // namespace NETWORK::WEB::NATIVE::SOCKET
