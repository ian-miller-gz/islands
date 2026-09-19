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
#include <network/instances/unix/instances.internal.hpp>

namespace INSTANCES = NETWORK::INSTANCES;

auto INSTANCES::list() -> const Vector<Entry>& { return STATE::entries; }

auto INSTANCES::GET::listings() -> Whole { return STATE::listings; }

auto INSTANCES::GET::peer() -> const String& { return STATE::peer; }

auto INSTANCES::receive() -> Vector<String> {
  Vector<String> lines;
  lines.swap(STATE::replies);
  return lines;
}

auto INSTANCES::GET::drops() -> Whole { return STATE::drops; }

auto INSTANCES::arrivals() -> Whole {
  Whole fresh = STATE::listings - STATE::shown;
  STATE::shown = STATE::listings;
  return fresh;
}

auto INSTANCES::departures() -> Whole {
  Whole fresh = STATE::drops - STATE::mourned;
  STATE::mourned = STATE::drops;
  return fresh;
}
