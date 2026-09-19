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
#include <island/shell/engine.internal.hpp>
#include <logger/filters.hpp>

namespace {
struct Named {
  STRING::Hot name;
  LOGGER::Level level;
};
constexpr Named LEVELS[] = {
  {"fatal", LOGGER::Level::fatal},  {"alert", LOGGER::Level::alert},
  {"crit", LOGGER::Level::crit},    {"error", LOGGER::Level::error},
  {"warn", LOGGER::Level::warn},    {"notice", LOGGER::Level::notice},
  {"info", LOGGER::Level::info},    {"debug", LOGGER::Level::debug},
  {"notset", LOGGER::Level::notset}};
}  // namespace

auto ISLAND::SHELL::LOGS::level() -> String {
  for (const auto &named : LEVELS)
    if (named.level == LOGGER::FILTERS::fallback) return String(named.name);
  return "notset";
}

auto ISLAND::SHELL::LOGS::set(const String &value) -> Flag {
  for (const auto &named : LEVELS)
    if (value == named.name)
      return LOGGER::FILTERS::fallback = named.level, true;
  return false;
}
