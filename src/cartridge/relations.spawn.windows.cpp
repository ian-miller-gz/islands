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
#include <logger.hpp>
#define LOGGER_CATEGORY "~/cartridge::relations"

#define NOMINMAX
#define NOUSER
#include <windows.h>

namespace RELATIONS::SPAWN {
namespace {

constexpr Whole NONE = 0;
constexpr Status REFUSED = 1;
constexpr Status INSISTED = 1;
constexpr DWORD ASKED = 0;

Vector<Instance> instances;

auto held(Whole process) -> Instance * {
  for (auto &instance : instances)
    if (instance.process == process) return &instance;
  return nullptr;
}

void release(Whole process) {
  for (Whole at = 0; at < instances.size(); ++at) {
    if (instances[at].process != process) continue;
    CloseHandle(instances[at].child);
    if (instances[at].job != nullptr) CloseHandle(instances[at].job);
    return static_cast<void>(instances.erase(instances.begin() + at));
  }
}

auto recorded(const Instance &instance, const String &bundle) -> Whole {
  if (instance.process == NONE) return NONE;
  instances.push_back(instance);
  children.push_back({instance.process, bundle});
  return instance.process;
}

}  // namespace
}  // namespace RELATIONS::SPAWN

auto RELATIONS::spawn(const String &host, const String &bundle) -> Whole {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  logger.debug(
    "Spawning %s --cartridge %s, owned.", host.c_str(), bundle.c_str());
  return SPAWN::recorded(SPAWN::launch(host, bundle, nullptr, true), bundle);
}

auto RELATIONS::spawn(
  const String &host, const String &bundle, const String &input) -> Whole {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  logger.debug(
    "Spawning %s --cartridge %s, owned, fed.", host.c_str(), bundle.c_str());
  SECURITY_ATTRIBUTES shared{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
  HANDLE reading = nullptr;
  HANDLE writing = nullptr;
  if (CreatePipe(&reading, &writing, &shared, 0) == 0) return SPAWN::NONE;
  SetHandleInformation(writing, HANDLE_FLAG_INHERIT, 0);
  const Whole child =
    SPAWN::recorded(SPAWN::launch(host, bundle, reading, true), bundle);
  CloseHandle(reading);
  DWORD wrote = 0;
  WriteFile(
    writing, input.data(), static_cast<DWORD>(input.size()), &wrote, nullptr);
  CloseHandle(writing);
  return child;
}

auto RELATIONS::start(const String &host, const String &bundle) -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  logger.debug(
    "Starting %s --cartridge %s, released.", host.c_str(), bundle.c_str());
  const Whole started = SPAWN::launch(host, bundle, nullptr, false).process;
  return started == SPAWN::NONE ? SPAWN::REFUSED : 0;
}

void RELATIONS::SPAWN::end(Whole process) {
  if (auto *instance = held(process))
    TerminateProcess(instance->child, INSISTED);
}

void RELATIONS::SPAWN::reap(Whole process) {
  auto *instance = held(process);
  if (instance == nullptr) return;
  WaitForSingleObject(instance->child, INFINITE);
  release(process);
}

Flag RELATIONS::SPAWN::ended(Whole process) {
  auto *instance = held(process);
  if (instance == nullptr) return true;
  if (WaitForSingleObject(instance->child, ASKED) != WAIT_OBJECT_0)
    return false;
  release(process);
  return true;
}
