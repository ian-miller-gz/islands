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

#include <shell/os/pty.hpp>

namespace SHELL::OS::PTY {
namespace {

constexpr Whole BLOCK = 4096;

Flag gathered(const Pty &pty, String &bytes) {
  const auto out = static_cast<HANDLE>(pty.out);
  char buffer[BLOCK];
  for (;;) {
    DWORD waiting = 0;
    if (!PeekNamedPipe(out, nullptr, 0, nullptr, &waiting, nullptr))
      return false;
    if (!waiting) return true;
    DWORD taken = 0;
    const auto block = static_cast<DWORD>(waiting < BLOCK ? waiting : BLOCK);
    if (!ReadFile(out, buffer, block, &taken, nullptr)) return false;
    bytes.append(buffer, static_cast<Whole>(taken));
  }
}

}  // namespace

void feed(Pty &pty, const String &bytes) {
  if (!pty.child) return;
  DWORD wrote = 0;
  WriteFile(
    static_cast<HANDLE>(pty.in), bytes.data(), static_cast<DWORD>(bytes.size()),
    &wrote, nullptr);
}

auto drain(Pty &pty) -> String {
  String bytes;
  if (!pty.child) return bytes;
  if (!gathered(pty, bytes)) stop(pty);
  return bytes;
}

}  // namespace SHELL::OS::PTY
