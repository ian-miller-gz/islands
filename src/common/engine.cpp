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
#include <generated/engine.hpp>
#include <common/types.hpp>
#include <common/constants.hpp>
#include <cstring>
#include <format>
#include <iostream>

static constexpr STRING::Hot WORD = "--version";

auto ENGINE::stated(int count, char **values) -> Flag {
  for (int at = 1; at < count; ++at) {
    if (std::strcmp(values[at], WORD) != 0) continue;
    std::cout << std::format("{} {}", ENGINE_NAME, VERSION) << std::endl;
    return true;
  }
  return false;
}
