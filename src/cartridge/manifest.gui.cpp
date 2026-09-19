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
#include <logger.hpp>
#include <algorithm>
#define LOGGER_CATEGORY "~/cartridge::"

auto CARTRIDGE::MANIFEST::designate(const String &line, Manifest &manifest)
  -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "manifest");
  String key, value;
  if (!STRING::pair(STRING::trim(line), key, value)) {
    logger.error("Malformed gui entry \"" + line + "\".");
    return 1;
  }
  auto &words = manifest.components;
  words.erase(std::remove(words.begin(), words.end(), key), words.end());
  if (value == "true" || value == "1") words.push_back(key);
  return 0;
}
