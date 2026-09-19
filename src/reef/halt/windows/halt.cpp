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

#include <reef/halt.hpp>

namespace REEF::HALT {
namespace {

using Handle = HANDLE;

Handle asked = nullptr;

BOOL WINAPI answer(DWORD) {
  if (asked) SetEvent(asked);
  return TRUE;
}

}  // namespace

void arm(Flag) {
  if (asked) return;
  asked = CreateEventW(nullptr, TRUE, FALSE, nullptr);
  SetConsoleCtrlHandler(answer, TRUE);
}

auto halted() -> Flag {
  return asked && WaitForSingleObject(asked, 0) == WAIT_OBJECT_0;
}

void await() {
  if (asked) WaitForSingleObject(asked, INFINITE);
}

void summon() {
  if (asked) SetEvent(asked);
}

}  // namespace REEF::HALT
