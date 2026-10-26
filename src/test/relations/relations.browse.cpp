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
#include <cartridge.hpp>

#include <cstdlib>
#include <iostream>

namespace {

constexpr STRING::Hot PAGE = "https://example.invalid/";
constexpr STRING::Hot LOCAL = "file:///dev/null";
constexpr STRING::Hot STANDIN = "true";
constexpr STRING::Hot MISSING = "absent";
constexpr STRING::Hot SEARCH = "PATH";
constexpr STRING::Hot EMPTY = "";
constexpr int REPLACE = 1;

auto said(STRING::Hot name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

}  // namespace

auto browsed() -> Status {
  const STRING::Hot found = std::getenv(SEARCH);
  const String path = found == nullptr ? EMPTY : found;
  setenv(CARTRIDGE::BROWSE::VARIABLE, STANDIN, REPLACE);
  Status status = said("standin started", CARTRIDGE::browse(PAGE) == 0);
  status |= said(
    "file address refused",
    CARTRIDGE::browse(LOCAL) == CARTRIDGE::BROWSE::ADDRESS);
  setenv(CARTRIDGE::BROWSE::VARIABLE, MISSING, REPLACE);
  setenv(SEARCH, EMPTY, REPLACE);
  status |= said(
    "missing opener refused",
    CARTRIDGE::browse(PAGE) == CARTRIDGE::BROWSE::OPENER);
  setenv(SEARCH, path.c_str(), REPLACE);
  unsetenv(CARTRIDGE::BROWSE::VARIABLE);
  return status;
}
