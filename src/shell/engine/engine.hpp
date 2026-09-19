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
#pragma once

#include <shell/shell.hpp>

namespace SHELL::ENGINE {

struct Setting {
  STRING::Hot key;
  String (*read)();
  Flag (*write)(const String &value);
};

struct Host {
  void (*summary)(Session &session) = nullptr;
  void (*quit)(Session &session) = nullptr;
  void (*reload)(Session &session) = nullptr;
  void (*version)(Session &session) = nullptr;
  void (*vsync)(Session &session) = nullptr;
  const Vector<Setting> *settings = nullptr;
  void (*status)(Session &session) = nullptr;
  const Vector<Command> *commands = nullptr;
  const Vector<Command> *debug = nullptr;
};

extern Host host;

void dispatch(Session &session);
void dispatch(Session &session, const String &line);

}  // namespace SHELL::ENGINE

namespace SHELL::ENGINE::GET {
auto commands() -> const Vector<Command> &;
auto monitors() -> const Vector<Command> &;
}  // namespace SHELL::ENGINE::GET
