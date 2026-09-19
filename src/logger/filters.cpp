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
#include <cctype>
#include <iostream>
#include <logger/filters.hpp>

using LOGGER::Level;

Level LOGGER::FILTERS::fallback = Level::info;
Map<String, Level> LOGGER::FILTERS::rules;

auto LOGGER::LEVELS::parse(const String &name) -> Level {
  static const Map<String, Level> names = {
    {"EMERG", Level::fatal},   {"FATAL", Level::fatal}, {"ALERT", Level::alert},
    {"CRIT", Level::crit},     {"ERROR", Level::error}, {"WARN", Level::warn},
    {"NOTICE", Level::notice}, {"INFO", Level::info},   {"DEBUG", Level::debug},
    {"NOTSET", Level::notset}, {"TRACE", Level::debug},
  };
  String upper = name;
  for (auto &letter : upper) {
    letter =
      static_cast<Char>(std::toupper(static_cast<unsigned char>(letter)));
  }
  auto rule = names.find(upper);
  if (rule != names.end()) return rule->second;
  std::cerr << "Unknown log level: " << name << std::endl;
  return FILTERS::fallback;
}

void LOGGER::FILTERS::add(const String &spec) {
  auto split = spec.find('=');
  if (split == String::npos) {
    fallback = LEVELS::parse(spec);
    return;
  }
  rules[spec.substr(0, split)] = LEVELS::parse(spec.substr(split + 1));
}
