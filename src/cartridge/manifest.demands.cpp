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
#include <cartridge/manifest.hpp>
#include <cartridge/manifest.internal.hpp>

auto CARTRIDGE::MANIFEST::demand(const String &line, Manifest &manifest)
  -> Status {
  String axis, value;
  if (!STRING::pair(line, axis, value)) return 0;
  Demand demand;
  demand.axis = axis;
  if (value.starts_with('[')) {
    auto close = value.find(']');
    value = value.substr(1, close == String::npos ? close : close - 1);
    for (const auto &member : STRING::split(value, ',')) {
      const String spelling = STRING::trim(member);
      if (!spelling.empty()) demand.values.push_back(spelling);
    }
  } else if (!value.empty())
    demand.values.push_back(value);
  if (!demand.values.empty()) manifest.demands.push_back(demand);
  return 0;
}
