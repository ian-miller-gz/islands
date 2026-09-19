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
#include <island/input.hpp>
#include <array>
#include <cctype>

constexpr Whole SPAN = INPUT::KEYS::FUNCTION + 13;

static std::array<Flag, SPAN> live{};
static std::array<Flag, SPAN> current{};
static std::array<Flag, SPAN> fresh{};
static Flag claimed = false;

static auto slot(Whole codepoint) -> Whole {
  if (codepoint >= INPUT::KEYS::FUNCTION)
    return codepoint < SPAN ? codepoint : 0;
  return static_cast<Whole>(std::toupper(static_cast<int>(codepoint)));
}

void INPUT::KEYS::press(Whole codepoint, Flag down) {
  if (codepoint < SPAN) live[codepoint] = down;
}

void INPUT::KEYS::claim(Flag owned) { claimed = owned; }

void INPUT::KEYS::frame() {
  for (Whole code = 0; code < SPAN; code += 1) {
    fresh[code] = live[code] && !current[code];
    current[code] = live[code];
  }
}

auto INPUT::GET::held(Whole codepoint) -> Flag {
  const auto code = slot(codepoint);
  return (code >= KEYS::FUNCTION || !claimed) && current[code];
}

auto INPUT::GET::pressed(Whole codepoint) -> Flag {
  const auto code = slot(codepoint);
  return (code >= KEYS::FUNCTION || !claimed) && fresh[code];
}
