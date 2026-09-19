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
#include <cartridge/axes.internal.hpp>

auto CARTRIDGE::AXES::token(const Entry &entry, const String &spelling)
  -> Whole {
  for (Whole at = 0; at < entry.count; at += 1)
    if (spelling == entry.values[at].spelling) return entry.values[at].token;
  return 0;
}

auto CARTRIDGE::AXES::carries(const Entry &entry, Whole token) -> Flag {
  if (entry.mask) return (entry.mask & (1u << token)) != 0;
  return token == entry.compiled;
}

auto CARTRIDGE::AXES::compiled(const Entry &entry) -> String {
  String out;
  for (Whole at = 0; at < entry.count; at += 1)
    if (carries(entry, entry.values[at].token)) {
      if (!out.empty()) out += ", ";
      out += entry.values[at].spelling;
    }
  return out;
}
