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
#include <signal.h>

#include <shell/os/pty.hpp>
#include <chrono>
#include <iostream>
#include <thread>

namespace PTY = SHELL::OS::PTY;

constexpr Whole TRIES = 400;
constexpr Whole TICK = 5;
static const String MARKER = "islands-pty-4242";

static void nap() {
  std::this_thread::sleep_for(std::chrono::milliseconds(TICK));
}

static String seen;

static Flag await(SHELL::OS::Pty &pty, const String &text) {
  for (Whole i = 0; i < TRIES; i += 1) {
    seen += PTY::drain(pty);
    if (seen.find(text) != String::npos) return true;
    nap();
  }
  return false;
}

static Flag reap(SHELL::OS::Pty &pty) {
  kill(pty.child, SIGHUP);
  for (Whole i = 0; i < TRIES; i += 1) {
    PTY::drain(pty);
    if (!PTY::running(pty)) return true;
    nap();
  }
  return false;
}

int main() {
  SHELL::OS::Pty pty;
  PTY::spawn(pty);
  if (!PTY::running(pty)) {
    std::cout << "pty: no interpreter tier (nothing spawned)" << std::endl;
    return 1;
  }
  PTY::feed(pty, "echo " + MARKER + "\n");
  PTY::feed(pty, "echo term-$TERM\n");
  const Flag echoed = await(pty, MARKER);
  const Flag termed = await(pty, "term-" + String(PTY::TERM));
  const Flag reaped = reap(pty);
  PTY::stop(pty);

  std::cout << "pty echo: " << (echoed ? "yes" : "no")
            << "  term: " << (termed ? "yes" : "no")
            << "  reap: " << (reaped ? "yes" : "no") << std::endl;
  return echoed && termed && reaped ? 0 : 1;
}
