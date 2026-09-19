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
constexpr DWORD IMMEDIATE = 0;
constexpr DWORD EMPTY = 0;
}  // namespace
}  // namespace TERMINAL::BACKEND::REPL

auto TERMINAL::BACKEND::REPL::pending() -> Flag {
  const HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
  if (input == nullptr || input == INVALID_HANDLE_VALUE) return false;
  if (GetFileType(input) == FILE_TYPE_PIPE) {
    DWORD waiting = EMPTY;
    if (!PeekNamedPipe(input, nullptr, 0, nullptr, &waiting, nullptr))
      return true;
    return waiting > EMPTY;
  }
  return WaitForSingleObject(input, IMMEDIATE) == WAIT_OBJECT_0;
}

auto TERMINAL::BACKEND::REPL::attached() -> Flag {
  return _isatty(_fileno(stdin)) && _isatty(_fileno(stdout));
}
