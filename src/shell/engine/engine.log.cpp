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
#include <shell/engine/engine.internal.hpp>
#include <logger.hpp>

static auto count(const SHELL::Session &session, Whole &out) -> Flag {
  if (session.arguments.size() < 2) return true;
  const String &token = session.arguments[1];
  if (token.empty()) return true;
  for (char digit : token)
    if (digit < '0' || digit > '9') return false;
  out = static_cast<Whole>(std::stoul(token));
  return true;
}

void SHELL::ENGINE::ACTIONS::log(Session &session) {
  const auto &arguments = session.arguments;
  if (
    arguments.size() >= 2 &&
    (arguments[1] == "--help" || arguments[1] == "-help"))
    return session.print("usage: " + arguments[0] + " [line count]");
  Whole lines = 0;
  if (!count(session, lines))
    return session.print("Unknown argument: " + arguments[1]);
  auto recent = LOGGER::TAP::recent(lines);
  if (recent.empty()) return session.print("No log lines recorded.");
  for (const auto &line : recent) session.print(line);
}
