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
#pragma once

#include <common.hpp>

namespace LOGGER {
enum class Level {
  fatal,
  alert,
  crit,
  error,
  warn,
  notice,
  info,
  debug,
  notset
};
}  // namespace LOGGER

namespace LOGGER::LEVELS {
auto parse(const String &name) -> Level;
}  // namespace LOGGER::LEVELS

namespace LOGGER::FILTERS {
extern Level fallback;
extern Map<String, Level> rules;
void add(const String &spec);
}  // namespace LOGGER::FILTERS
