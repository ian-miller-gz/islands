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
#include <network/web/loopback.link.internal.hpp>

namespace NETWORK::WEB::LOOPBACK {
namespace {
constexpr Whole DRAW = 1024;
constexpr Whole WORDS = 3;
constexpr Char SPACE = ' ';
constexpr Char MARK = '?';
constexpr STRING::Hot ROOT = "/";
constexpr STRING::Hot LINE = "\r\n";
constexpr STRING::Hot METHOD = "GET";
constexpr STRING::Hot VERSION = "HTTP/1.1 ";
constexpr STRING::Hot TYPE = "Content-Type: text/plain\r\n";
constexpr STRING::Hot LENGTH = "Content-Length: ";
constexpr STRING::Hot FIELDS =
  "Cache-Control: no-store\r\nConnection: close\r\n";
constexpr STRING::Hot ALLOW = "Allow: GET\r\n";

struct Verdict {
  Flag method;
  Flag path;
  STRING::Hot status;
  STRING::Hot owed;
};

constexpr Verdict VERDICTS[] = {
  {true, true, "200 OK", TYPE},
  {true, false, "404 Not Found", ""},
  {false, true, "405 Method Not Allowed", ALLOW},
  {false, false, "405 Method Not Allowed", ALLOW},
};

auto judged(Flag method, Flag path) -> const Verdict & {
  for (const Verdict &verdict : VERDICTS)
    if (verdict.method == method && verdict.path == path) return verdict;
  return VERDICTS[std::size(VERDICTS) - 1];
}

auto composed(const Verdict &verdict, const String &body) -> String {
  return VERSION + String(verdict.status) + LINE + verdict.owed + LENGTH +
         std::to_string(body.size()) + LINE + FIELDS + LINE + body;
}
}  // namespace

auto read(Link &link) -> Flag {
  Char taken[DRAW];
  const auto received = ::recv(link.descriptor, taken, sizeof(taken), 0);
  if (received > 0) link.pending.append(taken, static_cast<Whole>(received));
  return received > 0 || (received < 0 && failing() == AGAIN);
}

auto answered(const String &head, const Lane &one) -> Reply {
  const Vector<String> words =
    STRING::split(head.substr(0, head.find(LINE)), SPACE);
  if (words.size() != WORDS || !words[1].starts_with(ROOT)) return {};
  const auto mark = words[1].find(MARK);
  const Flag method = words[0] == METHOD;
  const Flag path = words[1].substr(0, mark) == one.path;
  const Verdict &verdict = judged(method, path);
  if (!method || !path) return {composed(verdict, {}), std::nullopt};
  const String query =
    mark == String::npos ? String() : words[1].substr(mark + 1);
  return {composed(verdict, one.answering(query)), query};
}
}  // namespace NETWORK::WEB::LOOPBACK
