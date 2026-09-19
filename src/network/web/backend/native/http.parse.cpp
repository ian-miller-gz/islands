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
#include <network/web/native.http.internal.hpp>

namespace NETWORK::WEB::NATIVE::HTTP {
namespace {
constexpr STRING::Hot BREAK = "\r\n";
constexpr Whole RELOCATIONS[]{301, 302, 303, 307, 308};

auto folded(const String &text) -> String {
  String lowered = text;
  for (Char &letter : lowered)
    letter =
      static_cast<Char>(std::tolower(static_cast<unsigned char>(letter)));
  return lowered;
}

auto stated(const String &line) -> Whole {
  auto space = line.find(' ');
  if (!line.starts_with("HTTP/") || space == String::npos) return 0;
  return number(line.substr(space + 1, 3));
}

void fields(const String &head, Vector<Header> &headers) {
  for (Whole mark = 0; mark < head.size();) {
    auto end = head.find(BREAK, mark);
    String line = head.substr(mark, end - mark);
    mark = (end == String::npos) ? head.size() : end + 2;
    auto colon = line.find(':');
    if (colon == String::npos) continue;
    auto value = line.find_first_not_of(' ', colon + 1);
    headers.push_back(
      {folded(line.substr(0, colon)),
       value == String::npos ? "" : line.substr(value)});
  }
}

auto valued(const Vector<Header> &headers, const String &name) -> String {
  for (const Header &header : headers)
    if (header.name == name) return header.value;
  return {};
}

auto relocated(Whole status) -> Flag {
  for (Whole known : RELOCATIONS)
    if (status == known) return true;
  return false;
}
}  // namespace

auto interpret(const String &wire, const Url &target, Result &result) -> Flag {
  auto split = wire.find(String(BREAK) + BREAK);
  if (split == String::npos) return false;
  String head = wire.substr(0, split);
  auto line = head.find(BREAK);
  result.status = stated(head.substr(0, line));
  if (!result.status) return false;
  Vector<Header> headers;
  if (line != String::npos) fields(head.substr(line + 2), headers);
  String body = wire.substr(split + 4);
  if (folded(valued(headers, "transfer-encoding")).contains("chunked")) {
    if (!unchunk(body, result.body)) return false;
  } else if (auto length = number(valued(headers, "content-length")); length) {
    result.body = body.substr(0, length);
  } else {
    result.body = body;
  }
  String place = valued(headers, "location");
  if (relocated(result.status) && !place.empty())
    result.redirect = rebase(target, place);
  result.ok = true;
  return true;
}
}  // namespace NETWORK::WEB::NATIVE::HTTP
