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
#define NOUSER

#include <shell/os/os.hpp>
#include <string>

#include <windows.h>

namespace SHELL::OS {
namespace {

using Wide = std::wstring;

constexpr Status FAILED = 1;

constexpr const wchar_t *SWITCH = L"\" /c ";

constexpr Char WRAP = '"';

auto interpreter() -> Wide {
  wchar_t named[MAX_PATH];
  const auto length = GetEnvironmentVariableW(L"COMSPEC", named, MAX_PATH);
  if (length == 0 || length >= MAX_PATH) return L"cmd.exe";
  return Wide(named, length);
}

auto widened(const String &text) -> Wide {
  const auto count = static_cast<int>(text.size());
  const auto length =
    MultiByteToWideChar(CP_UTF8, 0, text.data(), count, nullptr, 0);
  Wide wide(static_cast<Whole>(length), L'\0');
  MultiByteToWideChar(CP_UTF8, 0, text.data(), count, wide.data(), length);
  return wide;
}

}  // namespace

auto quote(const String &text) -> String {
  String quoted(1, WRAP);
  for (const Char letter : text) {
    if (letter == WRAP) quoted += WRAP;
    quoted += letter;
  }
  return quoted + WRAP;
}

auto run(const String &line) -> Status {
  const Wide named = interpreter();
  Wide command = Wide(1, WRAP) + named + SWITCH + widened(line);
  STARTUPINFOW startup{};
  startup.cb = sizeof startup;
  PROCESS_INFORMATION spawned{};
  if (!CreateProcessW(
        named.c_str(), command.data(), nullptr, nullptr, TRUE, 0, nullptr,
        nullptr, &startup, &spawned))
    return FAILED;
  CloseHandle(spawned.hThread);
  WaitForSingleObject(spawned.hProcess, INFINITE);
  DWORD status = FAILED;
  GetExitCodeProcess(spawned.hProcess, &status);
  CloseHandle(spawned.hProcess);
  return static_cast<Status>(status);
}

}  // namespace SHELL::OS
