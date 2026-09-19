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

#include <metrics.hpp>

namespace {

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

}  // namespace

int main() {
  using namespace METRICS;
  Status status = 0;
  reset();

#if SR_METRICS == SR_REGISTRY
  count("test.count");
  count("test.count", 2);
  status |=
    check("counter uncommitted before frame", GET::last("test.count") == 0);
  frame();
  status |= check("counter commits the pass sum", GET::last("test.count") == 3);

  gauge("test.gauge", 42);
  status |= check("gauge records the set value", GET::last("test.gauge") == 42);

  { Scope span("test.scope"); }
  status |=
    check("scope records one sample", GET::series("test.scope").size() == 1);

  for (Sample v : {10.0f, 20.0f, 30.0f, 40.0f}) gauge("test.fold", v);
  Fold fold = GET::fold("test.fold");
  status |= check("fold covers every sample", fold.samples == 4);
  status |= check(
    "fold orders low <= mean <= high",
    fold.min <= fold.mean && fold.mean <= fold.max);
  status |= check(
    "fold p99 within the window", fold.p99 >= fold.min && fold.p99 <= fold.max);

  status |= check("names answers every series", GET::names().size() == 4);
  std::cout << "series:" << std::endl;
  for (const auto &name : GET::names()) std::cout << "  " << name << std::endl;

  reset();
  status |= check("reset sweeps the registry", GET::names().empty());

  gauge("input.age", 0);
  gauge("frame.period", 0);
  count("gfx.draws", 1);
  frame();
  std::cout << "probes:" << std::endl;
  for (const auto &name : GET::names()) std::cout << "  " << name << std::endl;
  reset();

  memory();
  status |= check("memory samples the rss gauge", GET::last("memory.rss") > 0);
  status |= check(
    "memory tracks the peak >= rss",
    GET::last("memory.peak") >= GET::last("memory.rss"));
  std::cout << "memory:" << std::endl;
  for (const auto &name : GET::names()) std::cout << "  " << name << std::endl;

  arm(true);
  { Scope span("test.scope"); }
  arm(false);
  status |= check("hardware disarms to unarmed", !GET::armed());
#else
  count("test.count");
  count("test.count", 2);
  frame();
  gauge("test.gauge", 42);
  { Scope span("test.scope"); }
  memory();
  status |= check("counter records nothing", GET::last("test.count") == 0);
  status |= check("gauge records nothing", GET::last("test.gauge") == 0);
  status |= check("scope records nothing", GET::series("test.scope").empty());
  status |= check("fold reads all zero", GET::fold("test.fold").samples == 0);
  status |= check("memory records nothing", GET::last("memory.rss") == 0);
  status |= check("names reads empty", GET::names().empty());
  arm(true);
  status |= check("hardware stays unarmed", !GET::armed());
#endif

  return status;
}
