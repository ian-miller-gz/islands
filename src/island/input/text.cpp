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
#include <utility>

static Flag wants = false;
static Vector<INPUT::KEYS::Event> buffered;

void INPUT::TEXT::focus(Flag on) {
  wants = on;
  if (!on) buffered.clear();
}

auto INPUT::TEXT::wanted() -> Flag { return wants; }

void INPUT::TEXT::deliver(const KEYS::Event &event) {
  buffered.push_back(event);
}

void INPUT::TEXT::claim(Flag owned) {
  if (owned) focus(false);
}

auto INPUT::TEXT::drain() -> Vector<KEYS::Event> {
  return std::exchange(buffered, {});
}
