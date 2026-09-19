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

using Handle = HANDLE;
using Console = HPCON;

constexpr Handle IDLE = nullptr;

constexpr Whole INSISTED = 1;

void shut(void *&held) {
  if (held) CloseHandle(static_cast<Handle>(held));
  held = IDLE;
}

}  // namespace

Flag running(const Pty &pty) { return pty.child != IDLE; }

void resize(Pty &pty, Whole cols, Whole rows) {
  if (!pty.child || !pty.console) return;
  const COORD extent{static_cast<SHORT>(cols), static_cast<SHORT>(rows)};
  ResizePseudoConsole(static_cast<Console>(pty.console), extent);
}

void stop(Pty &pty) {
  if (pty.child) {
    const auto child = static_cast<Handle>(pty.child);
    TerminateProcess(child, static_cast<UINT>(INSISTED));
    WaitForSingleObject(child, INFINITE);
    shut(pty.child);
  }
  if (pty.console) ClosePseudoConsole(static_cast<Console>(pty.console));
  pty.console = IDLE;
  shut(pty.in);
  shut(pty.out);
}

}  // namespace SHELL::OS::PTY
