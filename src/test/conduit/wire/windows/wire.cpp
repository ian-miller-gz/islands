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
#define NOMINMAX
#include <windows.h>

#include <filesystem>
#include <string>

#include <test/conduit/conduit.internal.hpp>

namespace {
constexpr STRING::Hot FIXTURE = "conduit";

auto worded(const String &name) -> std::wstring {
  const std::filesystem::path self =
    std::filesystem::path(COMMON::PLATFORM::executable()) /
    COMMON::PLATFORM::binary(FIXTURE);
  const std::wstring peer(name.begin(), name.end());
  return L"\"" + self.wstring() + L"\" peer " + peer;
}
}  // namespace

auto wire() -> Flag {
  const String name = named("conduit.wire");
  const CONDUIT::Handle owner = CONDUIT::create(name, WIDTH, STRIDE);
  if (owner == CONDUIT::NONE) return false;
  std::wstring command = worded(name);
  STARTUPINFOW startup = {};
  startup.cb = sizeof(startup);
  PROCESS_INFORMATION child = {};
  Flag ok = CreateProcessW(
              nullptr, command.data(), nullptr, nullptr, FALSE, 0, nullptr,
              nullptr, &startup, &child) != 0;
  ok = ok && drained(owner);
  DWORD code = 1;
  ok = ok && WaitForSingleObject(child.hProcess, INFINITE) == WAIT_OBJECT_0;
  ok = ok && GetExitCodeProcess(child.hProcess, &code) != 0 && code == 0;
  if (child.hProcess != nullptr) {
    CloseHandle(child.hThread);
    CloseHandle(child.hProcess);
  }
  CONDUIT::destroy(owner);
  return ok;
}
