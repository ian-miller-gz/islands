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
#include <island/terminal/backend/repl.internal.hpp>

#include <cstdio>
#include <io.h>

#define NOMINMAX
#include <windows.h>

namespace TERMINAL::BACKEND::REPL {
namespace {
constexpr DWORD EMPTY = 0;
constexpr wchar_t ENTER = L'\r';

// A console handle signals for every input record — a mouse move, a focus
// change, a resize — none of which getline can consume, so a wait on the
// handle would send the read into a block. A line is pending only when an
// Enter key-down is queued.
auto lined(HANDLE input) -> Flag {
  DWORD queued = EMPTY;
  if (!GetNumberOfConsoleInputEvents(input, &queued) || queued == EMPTY)
    return false;
  Vector<INPUT_RECORD> records(queued);
  DWORD taken = EMPTY;
  if (!PeekConsoleInputW(input, records.data(), queued, &taken)) return false;
  for (DWORD at = 0; at < taken; ++at) {
    const auto &record = records[at];
    if (record.EventType != KEY_EVENT || !record.Event.KeyEvent.bKeyDown)
      continue;
    if (record.Event.KeyEvent.uChar.UnicodeChar == ENTER) return true;
  }
  return false;
}
}  // namespace
}  // namespace TERMINAL::BACKEND::REPL

auto TERMINAL::BACKEND::REPL::pending() -> Flag {
  const HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
  if (input == nullptr || input == INVALID_HANDLE_VALUE) return false;
  switch (GetFileType(input)) {
    case FILE_TYPE_PIPE: {
      DWORD waiting = EMPTY;
      if (!PeekNamedPipe(input, nullptr, 0, nullptr, &waiting, nullptr))
        return true;
      return waiting > EMPTY;
    }
    case FILE_TYPE_CHAR: return lined(input);
    case FILE_TYPE_DISK: return true;
    default: return false;
  }
}

auto TERMINAL::BACKEND::REPL::attached() -> Flag {
  return _isatty(_fileno(stdin)) && _isatty(_fileno(stdout));
}
