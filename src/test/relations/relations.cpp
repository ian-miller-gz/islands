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
#include <cartridge/relations.internal.hpp>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

namespace {

constexpr STRING::Hot HOST = "Reef";
constexpr STRING::Hot STAGED = ".out";
constexpr STRING::Hot BUNDLE = "cartridges/.test/absent";
constexpr STRING::Hot INPUT = "quit\n";
constexpr Whole PATIENCE = 1000;
constexpr Whole BEAT = 10;

auto check(STRING::Hot name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

auto host() -> String {
  const String staged = String(HOST) + STAGED;
  const std::filesystem::path beside(COMMON::BINARY);
  if (std::filesystem::exists(beside / COMMON::PLATFORM::binary(staged)))
    return staged;
  return HOST;
}

auto verbs(const String &host, const String &bundle) -> Status {
  const Whole child = RELATIONS::spawn(host, bundle);
  Status status = check("spawn answers a child", child != 0);
  status |= check("the child is an owned edge", RELATIONS::GET::alive(child));
  RELATIONS::SPAWN::end(child);
  RELATIONS::SPAWN::reap(child);
  status |= check("the reaped child has ended", RELATIONS::SPAWN::ended(child));
  RELATIONS::drop(child);
  return status;
}

auto sweep(const String &host, const String &bundle) -> Status {
  const Whole child = RELATIONS::spawn(host, bundle);
  Status status = check("spawn answers a child to sweep", child != 0);
  for (Whole at = 0; at < PATIENCE && RELATIONS::GET::alive(child); ++at) {
    RELATIONS::process();
    std::this_thread::sleep_for(std::chrono::milliseconds(BEAT));
  }
  return status |
         check(
           "the sweep drops a child that ended", !RELATIONS::GET::alive(child));
}

auto fed(const String &host, const String &bundle) -> Status {
  const Whole child = RELATIONS::spawn(host, bundle, INPUT);
  Status status = check("the fed spawn answers a child", child != 0);
  RELATIONS::kill(child);
  return status |
         check("kill drops the fed child", !RELATIONS::GET::alive(child));
}

auto released(const String &host, const String &bundle) -> Status {
  const Status started = RELATIONS::start(host, bundle);
  Status status = check("start launches a released instance", started == 0);
  return status | check(
                    "a released instance is no edge of the graph",
                    RELATIONS::children.empty());
}

auto walked(const String &host, const String &bundle) -> Status {
  const Whole child = RELATIONS::spawn(host, bundle);
  Status status = check("spawn answers a child to close", child != 0);
  RELATIONS::close();
  status |= check("the closed child is no edge", !RELATIONS::GET::alive(child));
  return status |
         check("the close walk clears the graph", RELATIONS::children.empty());
}

}  // namespace

int main() {
  const String named = host();
  const String bundle = BUNDLE;
  std::cout << "relations: " << named << " --cartridge " << bundle << std::endl;
  Status status = verbs(named, bundle);
  status |= sweep(named, bundle);
  status |= fed(named, bundle);
  status |= released(named, bundle);
  status |= walked(named, bundle);
  return status;
}
