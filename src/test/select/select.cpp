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
#include <common.hpp>
#include <iostream>
#include <islands/select.hpp>

namespace SELECT = ISLANDS::SELECT;

namespace {

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

const Vector<String> NAMES = {"alpha", "beta", "gamma"};
auto named(Whole index) -> const String & { return NAMES[index]; }

}  // namespace

auto main() -> int {
  const Whole count = NAMES.size();
  Status status = 0;

  status |= check("cycle NONE -> 0", SELECT::cycle(SELECT::NONE, count) == 0);
  status |= check("cycle 0 -> 1", SELECT::cycle(0, count) == 1);
  status |= check("cycle 2 -> 0 wrap", SELECT::cycle(2, count) == 0);
  status |= check("cycle empty -> NONE", SELECT::cycle(0, 0) == SELECT::NONE);

  status |=
    check("pick name beta -> 1", SELECT::pick("beta", count, named) == 1);
  status |= check("pick index 2 -> 2", SELECT::pick("2", count, named) == 2);
  status |= check(
    "pick unknown -> NONE", SELECT::pick("zeta", count, named) == SELECT::NONE);
  status |= check(
    "pick out-of-range index -> NONE",
    SELECT::pick("5", count, named) == SELECT::NONE);

  const Vector<String> lines = SELECT::listing(1, count, named, "  <-");
  status |= check("listing lines 3", lines.size() == 3);
  status |= check("listing line0 = '0: alpha'", lines[0] == "0: alpha");
  status |= check("listing line1 marked", lines[1] == "1: beta  <-");
  status |= check("listing line2 unmarked", lines[2] == "2: gamma");

  return status;
}
