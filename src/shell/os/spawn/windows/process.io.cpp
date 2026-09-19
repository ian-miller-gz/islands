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
#include <windows.h>

#include <shell/os/process.hpp>

namespace SHELL::OS::PROCESS {
namespace {

constexpr Whole BLOCK = 4096;

constexpr STRING::Hot GONE = "[shell exited]";

auto scrub(const String &line) -> String {
  String clean = line;
  for (auto &byte : clean)
    if (byte < ' ' || byte > '~') byte = ' ';
  return clean;
}

constexpr Char RETURN = '\r';

void split(Process &process, Vector<String> &lines) {
  Whole start = 0;
  auto &rest = process.rest;
  for (Whole at = 0; at < rest.size(); at += 1) {
    if (rest[at] != '\n') continue;
    Whole end = at;
    if (end > start && rest[end - 1] == RETURN) end -= 1;
    lines.push_back(scrub(rest.substr(start, end - start)));
    start = at + 1;
  }
  rest.erase(0, start);
}

Flag gathered(Process &process) {
  const auto out = static_cast<HANDLE>(process.out);
  char buffer[BLOCK];
  for (;;) {
    DWORD waiting = 0;
    if (!PeekNamedPipe(out, nullptr, 0, nullptr, &waiting, nullptr))
      return false;
    if (!waiting) return true;
    DWORD taken = 0;
    const auto block = static_cast<DWORD>(waiting < BLOCK ? waiting : BLOCK);
    if (!ReadFile(out, buffer, block, &taken, nullptr)) return false;
    process.rest.append(buffer, static_cast<Whole>(taken));
  }
}

}  // namespace

void feed(Process &process, const String &line) {
  if (!process.child) return;
  const String command = line + "\n";
  DWORD wrote = 0;
  WriteFile(
    static_cast<HANDLE>(process.in), command.data(),
    static_cast<DWORD>(command.size()), &wrote, nullptr);
}

auto harvest(Process &process) -> Vector<String> {
  Vector<String> ready;
  if (!process.child) return ready;
  const Flag ended = !gathered(process);
  split(process, ready);
  if (ended) {
    if (!process.rest.empty()) ready.push_back(scrub(process.rest));
    stop(process);
    ready.push_back(GONE);
  }
  return ready;
}

}  // namespace SHELL::OS::PROCESS
