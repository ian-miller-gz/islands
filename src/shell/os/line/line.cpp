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
#include <shell/os/os.hpp>
#include <console.hpp>

static SHELL::OS::Process child;

void SHELL::OS::adopt(CONSOLES::Console &console) {
  console.shell = &interpret;
  console.prompt = PROMPT;
}

void SHELL::OS::interpret(CONSOLES::Console &console, const String &line) {
  if (!PROCESS::running(child)) PROCESS::spawn(child);
  PROCESS::feed(child, line);
  (void)console;
}

void SHELL::OS::pump(CONSOLES::Console &console) {
  if (!PROCESS::running(child)) return;
  for (const auto &line : PROCESS::harvest(child))
    CONSOLES::append(console, line);
  if (PROCESS::running(child) || console.shell == console.home) return;
  console.shell = console.home;
  console.prompt = CONSOLES::PROMPT;
}

void SHELL::OS::close() { PROCESS::stop(child); }
