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
#include <network.hpp>
#include <shell/engine/engine.internal.hpp>
#include <string>

constexpr Whole GLIMPSE = 60;

static auto glimpse(const String &body) -> String {
  String line = body.substr(0, body.find('\n'));
  if (line.size() > GLIMPSE) line = line.substr(0, GLIMPSE);
  while (!line.empty() && (line.back() == '\r' || line.back() == ' '))
    line.pop_back();
  return line;
}

static void transfer(
  SHELL::Session &session, const String &url, const String &body) {
  auto result =
    body.empty() ? NETWORK::WEB::get(url) : NETWORK::WEB::put(url, body);
  if (!result.ok) return session.print(result.error);
  String seen = glimpse(result.body);
  session.print(
    "web " + std::to_string(result.status) + " (" +
    std::to_string(result.body.size()) + " bytes)" +
    (seen.empty() ? "." : ": " + seen));
}

void SHELL::ENGINE::ACTIONS::web(Session &session) {
  const auto &arguments = session.arguments;
  const String url = arguments.size() > 1 ? arguments[1] : "";
  String body;
  for (Whole i = 2; i < arguments.size(); i += 1)
    body += (body.empty() ? "" : " ") + arguments[i];
  if (url.empty() || url == "--help" || url == "-help")
    return session.print("usage: web <url> [body]");
  transfer(session, url, body);
}
