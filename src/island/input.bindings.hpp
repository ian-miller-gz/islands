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

namespace INPUT::BINDINGS {

struct Binding {
  STRING::Hot behavior;
  Whole ordinal;
};

constexpr Binding TABLE[] = {
  {"CONSOLES::DISPATCH::interpret", 10},
  {"CARTRIDGE::console", 9},
  {"WINDOW::fullscreen", 11},
  {"GUI::paste", 'V'},
  {"GUI::copy", 'C'},
};

constexpr auto match(STRING::Hot a, STRING::Hot b) -> Flag {
  while (*a && *a == *b) {
    ++a;
    ++b;
  }
  return *a == *b;
}

struct Directory {
  consteval auto operator[](STRING::Hot behavior) const -> Whole {
    for (const auto &binding : TABLE)
      if (match(behavior, binding.behavior)) return binding.ordinal;
    throw behavior;
  }
};
inline constexpr Directory keys{};

}  // namespace INPUT::BINDINGS
