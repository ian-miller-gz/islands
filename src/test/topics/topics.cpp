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
#include <iostream>

#include <topics.hpp>

static auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

#if SR_TOPICS == SR_LOCAL

static auto message(Whole tick) -> TOPICS::Message { return {{"tick", tick}}; }

static auto basics() -> Flag {
  TOPICS::reset();
  auto sub = TOPICS::subscribe("tick");
  TOPICS::publish("tick", message(1));
  TOPICS::publish("tick", message(2));
  auto got = TOPICS::drain(sub);
  return got.size() == 2 && std::get<Whole>(got[0].at("tick")) == 1 &&
         std::get<Whole>(got[1].at("tick")) == 2 && TOPICS::drain(sub).empty();
}

static auto routing() -> Flag {
  TOPICS::reset();
  auto a = TOPICS::subscribe("a"), b = TOPICS::subscribe("b");
  auto also = TOPICS::subscribe("a");
  TOPICS::publish("a", message(7));
  return TOPICS::drain(a).size() == 1 && TOPICS::drain(also).size() == 1 &&
         TOPICS::drain(b).empty();
}

static auto bounded() -> Flag {
  TOPICS::reset();
  auto sub = TOPICS::subscribe("flood");
  const Whole extra = 5, sent = TOPICS::BACKLOG + extra;
  for (Whole i = 0; i < sent; ++i) TOPICS::publish("flood", message(i));
  auto got = TOPICS::drain(sub);
  return got.size() == TOPICS::BACKLOG && TOPICS::GET::dropped(sub) == extra;
}

static auto lifecycle() -> Flag {
  TOPICS::reset();
  auto sub = TOPICS::subscribe("x");
  TOPICS::unsubscribe(sub);
  TOPICS::publish("x", message(0));
  Flag released = TOPICS::drain(sub).empty() && TOPICS::GET::dropped(sub) == 0;
  auto live = TOPICS::subscribe("x");
  TOPICS::publish("x", message(0));
  TOPICS::reset();
  Flag swept = TOPICS::drain(live).empty();
  TOPICS::Subscription none{};
  Flag safe = TOPICS::drain(none).empty() && TOPICS::GET::dropped(none) == 0;
  return released && swept && safe;
}

static auto scheduled() -> Flag {
  TOPICS::reset();
  auto sub = TOPICS::subscribe("bell");
  TOPICS::publish("bell", message(1), TOPICS::at(100));
  Flag early = TOPICS::deliver(50) == 0 && TOPICS::drain(sub).empty();
  Flag fired = TOPICS::deliver(100) == 1 &&
               std::get<Whole>(TOPICS::drain(sub).at(0).at("tick")) == 1;
  Flag once = TOPICS::deliver(200) == 0 && TOPICS::drain(sub).empty();
  const TOPICS::Instant base = TOPICS::now();
  TOPICS::publish("bell", message(2), TOPICS::after(1000));
  Flag relative = TOPICS::deliver(base) == 0 &&
                  TOPICS::deliver(base + 100000) == 1 &&
                  std::get<Whole>(TOPICS::drain(sub).at(0).at("tick")) == 2;
  return early && fired && once && relative;
}

static auto recurring() -> Flag {
  TOPICS::reset();
  auto sub = TOPICS::subscribe("beat");
  const TOPICS::Instant base = TOPICS::now();
  TOPICS::publish("beat", message(0), TOPICS::every(10));
  Flag pending = TOPICS::deliver(base) == 0;
  Flag collapsed =
    TOPICS::deliver(base + 10000) == 1 && TOPICS::drain(sub).size() == 1;
  Flag armed = TOPICS::deliver(base + 10000) == 0;
  Flag again =
    TOPICS::deliver(base + 10020) == 1 && TOPICS::drain(sub).size() == 1;
  return pending && collapsed && armed && again;
}

static auto ordered() -> Flag {
  TOPICS::reset();
  auto sub = TOPICS::subscribe("q");
  TOPICS::publish("q", message(50), TOPICS::at(50));
  TOPICS::publish("q", message(30), TOPICS::at(30));
  Flag both = TOPICS::deliver(100) == 2;
  auto got = TOPICS::drain(sub);
  return both && got.size() == 2 && std::get<Whole>(got[0].at("tick")) == 30 &&
         std::get<Whole>(got[1].at("tick")) == 50;
}

static auto purged() -> Flag {
  TOPICS::reset();
  TOPICS::subscribe("x");
  TOPICS::publish("x", message(0), TOPICS::at(50));
  TOPICS::reset();
  auto after = TOPICS::subscribe("x");
  return TOPICS::deliver(100) == 0 && TOPICS::drain(after).empty();
}

static auto run() -> Status {
  return check("basics", basics()) | check("routing", routing()) |
         check("bounded", bounded()) | check("lifecycle", lifecycle()) |
         check("scheduled", scheduled()) | check("recurring", recurring()) |
         check("ordered", ordered()) | check("purged", purged());
}

#else

static auto run() -> Status {
  auto sub = TOPICS::subscribe("gone");
  TOPICS::publish("gone", {{"tick", Whole{1}}});
  TOPICS::publish("gone", {{"tick", Whole{2}}}, TOPICS::after(1));
  TOPICS::publish("gone", {{"tick", Whole{3}}}, TOPICS::at(TOPICS::now()));
  TOPICS::publish("gone", {{"tick", Whole{4}}}, TOPICS::every(1));
  Flag scheduled = TOPICS::now() == 0 && TOPICS::deliver(1000000) == 0;
  TOPICS::reset();
  return check(
    "stubbed", sub.slot == 0 && TOPICS::drain(sub).empty() &&
                 TOPICS::GET::dropped(sub) == 0 && scheduled);
}

#endif

auto wire(const char *mode) -> Status;

auto main(int count, char **arguments) -> int {
  if (count > 1) return wire(arguments[1]);
  return run();
}
