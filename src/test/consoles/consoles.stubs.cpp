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
#include <shell/os/os.hpp>
#include <test/consoles/consoles.internal.hpp>
#include <chrono>
#include <thread>

void SHELL::ENGINE::dispatch(Session &session, const String &line) {
  session.print(line);
}

constexpr Whole TRIES = 400;
constexpr Whole TICK = 5;

static void nap() {
  std::this_thread::sleep_for(std::chrono::milliseconds(TICK));
}

void type(CONSOLES::Console &console, const String &line) {
  console.line = line;
  CONSOLES::submit(console);
}

Flag found(const CONSOLES::Console &console, const String &text) {
  for (const auto &line : console.lines)
    if (line == text) return true;
  return false;
}

Flag await(CONSOLES::Console &console, const String &text) {
  for (Whole i = 0; i < TRIES; i += 1) {
    SHELL::OS::pump(console);
    if (found(console, text)) return true;
    nap();
  }
  return false;
}

Flag reaps(SHELL::OS::Process &process, const String &text) {
  for (Whole i = 0; i < TRIES; i += 1) {
    for (const auto &line : SHELL::OS::PROCESS::harvest(process))
      if (line == text) return true;
    nap();
  }
  return false;
}

Flag settle(CONSOLES::Console &console) {
  for (Whole i = 0; i < TRIES; i += 1) {
    SHELL::OS::pump(console);
    if (console.shell == console.home) return true;
    nap();
  }
  return false;
}
