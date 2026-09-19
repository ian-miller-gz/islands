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
#include <shell/engine/engine.hpp>
#include <console/console.internal.hpp>

namespace {
struct Session final : SHELL::Session {
  CONSOLES::Console &console;
  explicit Session(CONSOLES::Console &owner) : console(owner) {}
  void print(const String &line) override { CONSOLES::append(console, line); }
  void clear() override { console.lines.clear(); }
  void close() override { console.visible = false; }
};
}  // namespace

void CONSOLES::DISPATCH::interpret(Console &console, const String &line) {
  Session session(console);
  SHELL::ENGINE::dispatch(session, line);
}
