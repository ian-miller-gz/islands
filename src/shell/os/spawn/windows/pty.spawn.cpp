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
#include <string>

namespace SHELL::OS::PTY {
namespace {

using Handle = HANDLE;
using Console = HPCON;
using Wide = std::wstring;

constexpr Handle IDLE = nullptr;

constexpr Whole CARRIED = 1;

auto interpreter() -> Wide {
  wchar_t named[MAX_PATH];
  const auto length = GetEnvironmentVariableW(L"COMSPEC", named, MAX_PATH);
  if (length == 0 || length >= MAX_PATH) return L"cmd.exe";
  return Wide(named, length);
}

Flag consoled(Pty &pty, Whole cols, Whole rows) {
  Handle fed = IDLE, feeding = IDLE, reaped = IDLE, reaping = IDLE;
  if (!CreatePipe(&fed, &feeding, nullptr, 0)) return false;
  if (!CreatePipe(&reaped, &reaping, nullptr, 0))
    return (void)(CloseHandle(fed), CloseHandle(feeding)), false;
  pty.in = feeding;
  pty.out = reaped;
  Console console = nullptr;
  const COORD extent{static_cast<SHORT>(cols), static_cast<SHORT>(rows)};
  const auto made = CreatePseudoConsole(extent, fed, reaping, 0, &console);
  CloseHandle(fed), CloseHandle(reaping);
  pty.console = console;
  return made == S_OK;
}

Flag attached(Console console, Vector<char> &store, STARTUPINFOEXW &startup) {
  SIZE_T bytes = 0;
  InitializeProcThreadAttributeList(nullptr, CARRIED, 0, &bytes);
  store.resize(bytes);
  startup.lpAttributeList =
    reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(store.data());
  if (!InitializeProcThreadAttributeList(
        startup.lpAttributeList, CARRIED, 0, &bytes))
    return false;
  return UpdateProcThreadAttribute(
           startup.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
           console, sizeof(Console), nullptr, nullptr) != 0;
}

}  // namespace

void spawn(Pty &pty) {
  if (pty.child) return;
  if (!consoled(pty, COLUMNS, ROWS)) return stop(pty);
  Vector<char> store;
  STARTUPINFOEXW startup{};
  startup.StartupInfo.cb = sizeof startup;
  Wide line = interpreter();
  PROCESS_INFORMATION spawned{};
  const auto made =
    attached(static_cast<Console>(pty.console), store, startup) &&
    CreateProcessW(
      nullptr, line.data(), nullptr, nullptr, FALSE,
      EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr, &startup.StartupInfo,
      &spawned);
  if (startup.lpAttributeList)
    DeleteProcThreadAttributeList(startup.lpAttributeList);
  if (!made) return stop(pty);
  CloseHandle(spawned.hThread);
  pty.child = spawned.hProcess;
}

}  // namespace SHELL::OS::PTY
