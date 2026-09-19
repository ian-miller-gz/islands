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
#include <islands/aabb.hpp>
#include <islands/tilemap.hpp>

namespace AABB = ISLANDS::AABB;
namespace TILEMAP = ISLANDS::TILEMAP;

namespace {

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

constexpr Whole COLUMNS = 8, CELL = 16;
constexpr STRING::Hot LINES[] = {
  "#......#", "#..##..#", "#......#", "#.o....#", "########"};

auto build() -> TILEMAP::Grid {
  const Vector<TILEMAP::Code> table = {
    {'#', {0, 0, true}}, {'o', {1, 0, false}}};
  const Vector<STRING::Hot> lines(LINES, LINES + 5);
  return TILEMAP::decode(lines, COLUMNS, CELL, CELL, table);
}

}  // namespace

auto main() -> int {
  Status status = 0;

  const AABB::Box unit{0, 0, 16, 16};
  status |= check("overlap self", AABB::overlap(unit, unit));
  status |= check("overlap nested", AABB::overlap(unit, AABB::Box{4, 4, 4, 4}));
  status |=
    check("overlap partial", AABB::overlap(unit, AABB::Box{8, 8, 16, 16}));
  status |= check(
    "overlap touch-right strict", !AABB::overlap(unit, AABB::Box{16, 0, 8, 8}));
  status |= check(
    "overlap touch-below strict", !AABB::overlap(unit, AABB::Box{0, 16, 8, 8}));
  status |=
    check("overlap disjoint", !AABB::overlap(unit, AABB::Box{32, 0, 8, 8}));

  status |= check("contains interior", AABB::contains(unit, 8, 8));
  status |= check("contains top-left edge in", AABB::contains(unit, 0, 0));
  status |= check("contains right edge out", !AABB::contains(unit, 16, 8));
  status |= check("contains bottom edge out", !AABB::contains(unit, 8, 16));
  status |= check("contains outside", !AABB::contains(unit, 20, 20));

  const TILEMAP::Grid grid = build();
  status |= check("grid hit corner wall", AABB::overlap({0, 0, 16, 16}, grid));
  status |= check("grid miss gap", !AABB::overlap({32, 0, 16, 16}, grid));
  status |=
    check("grid straddle wall+gap", AABB::overlap({8, 0, 16, 16}, grid));
  status |=
    check("grid hit floating block", AABB::overlap({48, 16, 16, 16}, grid));
  status |=
    check("grid miss above block", !AABB::overlap({48, 0, 16, 15}, grid));
  status |=
    check("grid miss over marker", !AABB::overlap({32, 48, 16, 16}, grid));
  status |= check("grid hit floor", AABB::overlap({112, 72, 16, 8}, grid));
  status |=
    check("grid miss past edge", !AABB::overlap({200, 8, 16, 16}, grid));
  status |=
    check("grid hit clipped at origin", AABB::overlap({-8, -8, 16, 16}, grid));

  return status;
}
