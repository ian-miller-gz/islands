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

void SHELL::ENGINE::ACTIONS::echo(Session &session) {
  const auto &arguments = session.arguments;
  if (arguments.size() < 2) return session.print("");
  if (arguments[1] == "--help" || arguments[1] == "-help")
    return session.print("usage: " + arguments[0] + " [text to be printed]");
  if (arguments[1][0] == '-')
    return session.print("Unknown argument: " + arguments[1]);
  String text;
  for (Whole index = 1; index < arguments.size(); ++index) {
    if (arguments[index].empty()) continue;
    if (!text.empty()) text += ' ';
    text += arguments[index];
  }
  session.print(text);
}
