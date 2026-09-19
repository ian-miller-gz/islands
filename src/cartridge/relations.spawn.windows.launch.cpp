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
#include <cartridge/relations.spawn.windows.internal.hpp>
#include <common/constants.hpp>
#include <common/platform/paths.hpp>

#include <filesystem>
#include <string>

#define NOMINMAX
#define NOUSER
#include <windows.h>

namespace RELATIONS::SPAWN {
namespace {

using Handle = HANDLE;
using Wide = std::wstring;

const Handle NOTHING = INVALID_HANDLE_VALUE;
constexpr Whole NONE = 0;
constexpr const wchar_t *SINK = L"NUL";
constexpr DWORD HELD = CREATE_SUSPENDED;
constexpr DWORD RELEASED = CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS;

auto worded(const String &host, const String &bundle) -> Wide {
  const std::filesystem::path binary =
    std::filesystem::path(COMMON::BINARY) / COMMON::PLATFORM::binary(host);
  return L"\"" + binary.wstring() + L"\" --cartridge \"" +
         std::filesystem::path(bundle).wstring() + L"\"";
}

auto bound() -> Handle {
  Handle job = CreateJobObjectW(nullptr, nullptr);
  if (job == nullptr) return nullptr;
  JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {};
  limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
  SetInformationJobObject(
    job, JobObjectExtendedLimitInformation, &limits, sizeof(limits));
  return job;
}

Flag started(
  const Wide &command, Handle input, DWORD flags, PROCESS_INFORMATION &child) {
  SECURITY_ATTRIBUTES shared{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
  Handle sunk = CreateFileW(
    SINK, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
    &shared, OPEN_EXISTING, 0, nullptr);
  Wide line = command;
  STARTUPINFOW startup = {};
  startup.cb = sizeof(startup);
  startup.dwFlags = STARTF_USESTDHANDLES;
  startup.hStdInput = input == nullptr ? sunk : static_cast<Handle>(input);
  startup.hStdOutput = sunk;
  startup.hStdError = sunk;
  const Flag ok = CreateProcessW(
                    nullptr, line.data(), nullptr, nullptr, TRUE, flags,
                    nullptr, nullptr, &startup, &child) != 0;
  if (sunk != NOTHING) CloseHandle(sunk);
  return ok;
}

}  // namespace
}  // namespace RELATIONS::SPAWN

auto RELATIONS::SPAWN::launch(
  const String &host, const String &bundle, void *input,
  Flag owned) -> Instance {
  Handle job = owned ? bound() : nullptr;
  PROCESS_INFORMATION child = {};
  if (!started(
        worded(host, bundle), static_cast<Handle>(input),
        owned ? HELD : RELEASED, child)) {
    if (job != nullptr) CloseHandle(job);
    return {NONE, nullptr, nullptr};
  }
  if (job != nullptr) AssignProcessToJobObject(job, child.hProcess);
  if (owned) ResumeThread(child.hThread);
  CloseHandle(child.hThread);
  if (owned)
    return {static_cast<Whole>(child.dwProcessId), child.hProcess, job};
  CloseHandle(child.hProcess);
  return {static_cast<Whole>(child.dwProcessId), nullptr, nullptr};
}
