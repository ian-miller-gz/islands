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
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>

#include <topics/durable.hpp>

namespace {

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

#if SR_TOPICS == SR_LOCAL

constexpr TOPICS::Instant EARLY = 900;
constexpr TOPICS::Instant FILED = 1000;
constexpr TOPICS::Instant PASSED = 2000;
constexpr TOPICS::Instant LATER = 9000;
constexpr Integer PERIOD = 100;

auto message(Whole tick) -> TOPICS::Message { return {{"tick", tick}}; }

auto file(STORE::Handle store) -> Status {
  using namespace TOPICS;
  DURABLE::publish(store, "once", "due", message(1), at(EARLY));
  DURABLE::publish(store, "cycle", "due", message(2), Schedule{FILED, PERIOD});
  DURABLE::publish(store, "later", "quiet", message(3), at(LATER));
  DURABLE::publish(store, "dropped", "quiet", message(4), at(FILED));
  DURABLE::forget(store, "dropped");
  DURABLE::publish(store, "later", "quiet", message(3), at(LATER));
  auto filed = DURABLE::GET::schedules(store);
  return check(
    "filed", filed.size() == 3 && filed[0] == "cycle" && filed[1] == "later" &&
               filed[2] == "once" &&
               DURABLE::GET::due(store, "later") == LATER);
}

auto deliver(STORE::Handle store) -> Status {
  using namespace TOPICS;
  Status status = 0;
  auto due = subscribe("due"), quiet = subscribe("quiet");
  Whole released = DURABLE::deliver(store, PASSED);
  auto got = drain(due);
  status |= check(
    "survived", released == 2 && got.size() == 2 &&
                  std::get<Whole>(got[0].at("tick")) == 1 &&
                  std::get<Whole>(got[1].at("tick")) == 2 &&
                  drain(quiet).empty());
  auto left = DURABLE::GET::schedules(store);
  status |= check(
    "rearmed", left.size() == 2 && left[0] == "cycle" && left[1] == "later" &&
                 DURABLE::GET::due(store, "cycle") > PASSED &&
                 DURABLE::GET::due(store, "once") == 0);
  status |= check(
    "settled", DURABLE::deliver(store, PASSED) == 0 && drain(due).empty());
  reset();
  return status;
}

#else

auto file(STORE::Handle store) -> Status {
  TOPICS::DURABLE::publish(store, "once", "due", {}, TOPICS::at(1));
  return check("stubbed", TOPICS::DURABLE::GET::schedules(store).empty());
}

auto deliver(STORE::Handle store) -> Status {
  return check(
    "stubbed", TOPICS::DURABLE::deliver(store, 1) == 0 &&
                 TOPICS::DURABLE::GET::due(store, "once") == 0);
}

#endif

}  // namespace

auto main(int count, char **arguments) -> int {
  if (count < 3) {
    std::cout << "FAIL usage: durable <home> file|deliver" << std::endl;
    return 1;
  }
  std::filesystem::create_directories(arguments[1]);
  COMMON::PLATFORM::assign("XDG_DATA_HOME", arguments[1]);
  auto store = STORE::open("durable-fixture");
  if (store.id == 0) return check("opened", false);
  Status status =
    std::strcmp(arguments[2], "file") == 0 ? file(store) : deliver(store);
  STORE::close(store);
  return status;
}
