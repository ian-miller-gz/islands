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
#include <console/console.internal.hpp>
#include <shell/os/os.hpp>
#include <test/consoles/consoles.internal.hpp>
#include <iostream>

static Flag shells() {
  SHELL::OS::Process one, two;
  SHELL::OS::PROCESS::spawn(one);
  SHELL::OS::PROCESS::spawn(two);
  SHELL::OS::PROCESS::feed(one, "echo one");
  SHELL::OS::PROCESS::feed(two, "echo two");
  if (!reaps(one, "one") || !reaps(two, "two")) return false;
  const Flag live =
    SHELL::OS::PROCESS::running(one) && SHELL::OS::PROCESS::running(two);
  SHELL::OS::PROCESS::stop(one);
  SHELL::OS::PROCESS::stop(two);
  return live;
}

static Flag modes() {
  auto &console = CONSOLES::GET::console();
  SHELL::OS::adopt(console);
  if (console.shell == &CONSOLES::DISPATCH::interpret) return false;
  if (String(console.prompt) != SHELL::OS::PROMPT) return false;
  type(console, "echo mode");
  if (!await(console, "mode")) return false;
  type(console, "exit");
  if (!settle(console)) return false;
  return console.shell == &CONSOLES::DISPATCH::interpret &&
         String(console.prompt) == CONSOLES::PROMPT &&
         found(console, "[shell exited]");
}

static Flag runs() {
  if (SHELL::OS::run("exit 0") != 0) return false;
  if (SHELL::OS::run("exit 1") == 0) return false;
  return SHELL::OS::run("islands-no-such-tool") != 0;
}

static Status check(STRING::Hot name, Flag passed) {
  std::cout << "consoles " << name << ": " << (passed ? "PASS" : "FAIL")
            << std::endl;
  return passed ? 0 : 1;
}

int main() {
  CONSOLES::initialize();
  Status status = check("shells", shells());
  status |= check("modes", modes());
  status |= check("runs", runs());
  status |= check("guests", guests());
  status |= check("pages", pages());
  SHELL::OS::close();
  CONSOLES::close();
  return status;
}
