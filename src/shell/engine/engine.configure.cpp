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

void SHELL::ENGINE::ACTIONS::configure(Session &session) {
  const auto &arguments = session.arguments;
  const Vector<Setting> *settings = host.settings;
  if (!settings || settings->empty())
    return session.print("Nothing to configure.");
  if (arguments.size() < 2) {
    for (const auto &setting : *settings)
      session.print(String(setting.key) + " = " + setting.read());
    return;
  }
  if (arguments[1] == "--help" || arguments[1] == "-help")
    return session.print("usage: " + arguments[0] + " [key [value]]");
  for (const auto &setting : *settings) {
    if (arguments[1] != setting.key) continue;
    if (arguments.size() < 3)
      return session.print(String(setting.key) + " = " + setting.read());
    if (!setting.write)
      return session.print(String(setting.key) + " is read-only.");
    if (!setting.write(arguments[2]))
      return session.print("Unknown value: " + arguments[2]);
    return session.print(String(setting.key) + " = " + setting.read());
  }
  session.print("Unknown setting: " + arguments[1]);
}
