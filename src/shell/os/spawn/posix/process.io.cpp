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
#include <unistd.h>

#include <shell/os/process.hpp>

namespace PROCESS = SHELL::OS::PROCESS;

static String scrub(const String &line) {
  String clean = line;
  for (auto &byte : clean)
    if (byte < ' ' || byte > '~') byte = ' ';
  return clean;
}

static void split(SHELL::OS::Process &process, Vector<String> &lines) {
  Whole start = 0;
  auto &rest = process.rest;
  for (Whole i = 0; i < rest.size(); i += 1) {
    if (rest[i] != '\n') continue;
    lines.push_back(scrub(rest.substr(start, i - start)));
    start = i + 1;
  }
  rest.erase(0, start);
}

void PROCESS::feed(Process &process, const String &line) {
  if (process.child <= 0) return;
  const String command = line + "\n";
  if (write(process.in, command.data(), command.size()) < 0) return;
}

auto PROCESS::harvest(Process &process) -> Vector<String> {
  Vector<String> ready;
  if (process.child <= 0) return ready;
  Flag ended = false;
  char buffer[4096];
  for (;;) {
    const auto count = read(process.out, buffer, sizeof buffer);
    if (count > 0) {
      process.rest.append(buffer, static_cast<Whole>(count));
      continue;
    }
    if (count == 0) ended = true;
    break;
  }
  split(process, ready);
  if (ended) {
    if (!process.rest.empty()) ready.push_back(scrub(process.rest));
    stop(process);
    ready.push_back("[shell exited]");
  }
  return ready;
}
