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

#include <shell/os/pty.hpp>
#include <cerrno>

namespace PTY = SHELL::OS::PTY;

void PTY::feed(Pty &pty, const String &bytes) {
  if (pty.child <= 0) return;
  if (write(pty.master, bytes.data(), bytes.size()) < 0) return;
}

auto PTY::drain(Pty &pty) -> String {
  String bytes;
  if (pty.child <= 0) return bytes;
  Flag ended = false;
  char buffer[4096];
  for (;;) {
    const auto count = read(pty.master, buffer, sizeof buffer);
    if (count > 0) {
      bytes.append(buffer, static_cast<Whole>(count));
      continue;
    }
    if (count == 0) {
      ended = true;
      break;
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
    ended = true;
    break;
  }
  if (ended) stop(pty);
  return bytes;
}
