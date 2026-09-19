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
#include <island/terminal/backend/unix/listen.internal.hpp>
#include <logger.hpp>
#include <sstream>

#if SR_LISTEN == SR_UNIX

static void toggle(SHELL::Session &session, const String &argument) {
  using namespace TERMINAL::BACKEND::LISTEN;
  if (argument == "on") {
    if (LOGGER::TAP::open())
      return session.print("Logs require the manifest logs key or --logs 1.");
    streaming = true;
    return session.print("Logs on.");
  }
  if (streaming) LOGGER::TAP::close();
  streaming = false;
  session.print("Logs off.");
}

auto TERMINAL::BACKEND::LISTEN::intercept(
  SHELL::Session &session, const String &line) -> Flag {
  std::istringstream reader(line);
  String word, argument;
  reader >> word;
  if (word != "logs") return false;
  reader >> argument;
  if (argument.empty())
    session.print(streaming ? "Logs are on." : "Logs are off.");
  else if (argument == "--help" || argument == "-help")
    session.print("usage: " + word + " [on|off]");
  else if (argument == "on" || argument == "off")
    toggle(session, argument);
  else
    session.print("Unknown argument: " + argument);
  return true;
}

void TERMINAL::BACKEND::LISTEN::stream() {
  if (session == NETWORK::NONE || !streaming) return;
  for (const auto &line : LOGGER::TAP::receive()) {
    send(line + "\n");
    if (session == NETWORK::NONE) return;
  }
}

#endif
