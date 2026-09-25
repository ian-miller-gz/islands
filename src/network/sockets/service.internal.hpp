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
#include <cstdint>

#include <common.hpp>

namespace NETWORK::SERVICE {
constexpr uint32_t BASE = 49152;
constexpr uint32_t RANGE = 16383;

inline auto port(const String &service) -> Whole {
  uint32_t hash = 2166136261u;
  for (unsigned char byte : service) hash = (hash ^ byte) * 16777619u;
  return BASE + hash % RANGE;
}
}  // namespace NETWORK::SERVICE
