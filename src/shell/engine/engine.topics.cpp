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
#include <topics.hpp>
#include <format>

#if SR_TOPICS == SR_LOCAL

static void peek(SHELL::Session &session) {
  const String &name = session.arguments[2];
  Whole subscribers = TOPICS::GET::subscribers(name.c_str());
  if (subscribers == 0) return session.print("No such topic: " + name);
  session.print(std::format(
    "topic {}: {} subscribers, {} pending", name, subscribers,
    TOPICS::GET::pending(name.c_str())));
}

static void list(SHELL::Session &session) {
  auto names = TOPICS::GET::names();
  if (names.empty()) return session.print("No topics.");
  for (const auto &name : names) session.print(name);
}

void SHELL::ENGINE::ACTIONS::topics(Session &session) {
  const auto &arguments = session.arguments;
  if (
    arguments.size() >= 2 &&
    (arguments[1] == "--help" || arguments[1] == "-help"))
    return session.print("usage: topics [peek <name>]");
  if (arguments.size() >= 2 && arguments[1] == "peek") {
    if (arguments.size() < 3) return session.print("usage: topics peek <name>");
    return peek(session);
  }
  if (arguments.size() >= 2 && arguments[1] != "list")
    return session.print("Unknown argument: " + arguments[1]);
  list(session);
}

#endif
