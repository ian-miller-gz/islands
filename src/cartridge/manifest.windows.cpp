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
#define LOGGER_CATEGORY "~/cartridge::"

auto CARTRIDGE::MANIFEST::window(const String &line, Manifest &manifest)
  -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY "manifest");
  String body = STRING::trim(line);
  if (body.starts_with('-')) {
    manifest.windows.emplace_back();
    body = STRING::trim(body.substr(1));
  }
  if (body.empty()) return 0;
  String key, value;
  if (!STRING::pair(body, key, value) || manifest.windows.empty()) {
    logger.error("Malformed windows entry \"" + line + "\".");
    return 1;
  }
  WindowSpec &spec = manifest.windows.back();
  if (key == "name")
    spec.name = value;
  else if (key == "title")
    spec.title = value;
  else if (key == "show")
    spec.show = value == "true" || value == "1";
  else {
    logger.error("Unknown windows field \"" + key + "\".");
    return 1;
  }
  return 0;
}
