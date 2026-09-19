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
#include <network.hpp>

#if SR_LISTEN == SR_UNIX

static void list(SHELL::Session &session) {
  session.print(
    NETWORK::INSTANCES::refresh() ? "Listing..." : "No roster daemon.");
}

static void send(SHELL::Session &session) {
  const auto &arguments = session.arguments;
  if (arguments.size() < 4)
    return session.print("usage: instances send <pid> <line>");
  const String &pid = arguments[2];
  if (NETWORK::INSTANCES::GET::peer() != pid) {
    Flag listed = false;
    for (const auto &entry : NETWORK::INSTANCES::list())
      if (entry.pid == pid) {
        listed = true;
        break;
      }
    if (!listed) return session.print("Unknown instance: " + pid);
    if (!NETWORK::INSTANCES::attach(pid))
      return session.print("Cannot reach " + pid + ".");
  }
  String line = arguments[3];
  for (Whole at = 4; at < arguments.size(); ++at) line += " " + arguments[at];
  if (!NETWORK::INSTANCES::send(line)) session.print("Not attached.");
}

void SHELL::ENGINE::ACTIONS::instances(Session &session) {
  const auto &arguments = session.arguments;
  if (
    arguments.size() >= 2 &&
    (arguments[1] == "--help" || arguments[1] == "-help"))
    return session.print("usage: instances [send <pid> <line>]");
  if (arguments.size() >= 2 && arguments[1] == "send") return send(session);
  if (arguments.size() >= 2 && arguments[1] != "list")
    return session.print("Unknown argument: " + arguments[1]);
  list(session);
}

#endif
