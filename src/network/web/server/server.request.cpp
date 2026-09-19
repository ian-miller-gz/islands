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
#include <algorithm>
#include <cctype>

namespace NETWORK::WEB::SERVER {
namespace {
constexpr STRING::Hot LINE = "\r\n";
const String FIELD = "authorization:";

auto refuse(Whole status) -> Sending {
  return {{status, "text/plain; charset=utf-8", {}}, true};
}

auto credential(const Vector<String> &lines) -> String {
  for (Whole index = 1; index < lines.size(); ++index) {
    String name = lines[index].substr(0, FIELD.size());
    std::transform(name.begin(), name.end(), name.begin(), [](Char letter) {
      return static_cast<Char>(
        std::tolower(static_cast<unsigned char>(letter)));
    });
    if (name == FIELD) return STRING::trim(lines[index].substr(FIELD.size()));
  }
  return {};
}
}  // namespace

auto answered(const String &head, Handler answering) -> Sending {
  const Vector<String> lines = STRING::split(head, String(LINE));
  const Vector<String> words = STRING::split(lines.front(), ' ');
  if (words.size() != 3 || !words[1].starts_with("/")) return refuse(400);
  const Flag body = words[0] != "HEAD";
  if (words[0] != "GET" && words[0] != "HEAD") return refuse(405);
  const String reader = resolve(credential(lines));
  if (reader.empty()) return refuse(401);
  const auto mark = words[1].find('?');
  Request ask{
    reader, words[1].substr(0, mark),
    mark == String::npos ? String() : words[1].substr(mark + 1)};
  return {answering(ask), body};
}

}  // namespace NETWORK::WEB::SERVER
