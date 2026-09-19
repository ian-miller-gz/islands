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
#include <logger/filters.hpp>

namespace LOGGER {

class Category {
 public:
  static auto getRoot() -> Category &;

  void debug(const String &message);
  void info(const String &message);
  void warn(const String &message);
  void error(const String &message);
  void fatal(const String &message);

  void debug(const char *format, ...);
  void info(const char *format, ...);
  void warn(const char *format, ...);
  void error(const char *format, ...);
  void fatal(const char *format, ...);

 private:
  void *native = nullptr;
  friend auto get(const String &name) -> Category &;
};

void initialize();

void reset();

auto get(const String &name) -> Category &;

}  // namespace LOGGER

namespace LOGGER::GET {

auto path() -> const String &;

}  // namespace LOGGER::GET
