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
#include <string>

namespace SHELL::OS::PROCESS {
namespace {

using Handle = HANDLE;
using Wide = std::wstring;

constexpr Handle IDLE = nullptr;

constexpr Whole INSISTED = 1;

constexpr const wchar_t *QUIET = L"\" /q";

constexpr wchar_t WRAP = L'"';

auto interpreter() -> Wide {
  wchar_t named[MAX_PATH];
  const auto length = GetEnvironmentVariableW(L"COMSPEC", named, MAX_PATH);
  const Wide seat =
    length == 0 || length >= MAX_PATH ? Wide(L"cmd.exe") : Wide(named, length);
  return Wide(1, WRAP) + seat + QUIET;
}

Flag piped(Handle &reading, Handle &writing) {
  SECURITY_ATTRIBUTES shared{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
  return CreatePipe(&reading, &writing, &shared, 0) != 0;
}

void shut(void *&held) {
  if (held) CloseHandle(static_cast<Handle>(held));
  held = IDLE;
}

}  // namespace

Flag running(const Process &process) { return process.child != IDLE; }

void spawn(Process &process) {
  if (process.child) return;
  Handle fed = IDLE, feeding = IDLE, reaped = IDLE, reaping = IDLE;
  if (!piped(fed, feeding)) return;
  if (!piped(reaped, reaping))
    return (void)(CloseHandle(fed), CloseHandle(feeding));
  SetHandleInformation(feeding, HANDLE_FLAG_INHERIT, 0);
  SetHandleInformation(reaped, HANDLE_FLAG_INHERIT, 0);
  STARTUPINFOW startup{};
  startup.cb = sizeof startup;
  startup.dwFlags = STARTF_USESTDHANDLES;
  startup.hStdInput = fed;
  startup.hStdOutput = startup.hStdError = reaping;
  PROCESS_INFORMATION spawned{};
  Wide line = interpreter();
  const auto made = CreateProcessW(
    nullptr, line.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr,
    nullptr, &startup, &spawned);
  CloseHandle(fed), CloseHandle(reaping);
  if (!made) return (void)(CloseHandle(feeding), CloseHandle(reaped));
  CloseHandle(spawned.hThread);
  process.child = spawned.hProcess;
  process.in = feeding;
  process.out = reaped;
}

void stop(Process &process) {
  if (!process.child) return;
  shut(process.in);
  shut(process.out);
  const auto child = static_cast<Handle>(process.child);
  TerminateProcess(child, static_cast<UINT>(INSISTED));
  WaitForSingleObject(child, INFINITE);
  shut(process.child);
  process.rest.clear();
}

}  // namespace SHELL::OS::PROCESS
