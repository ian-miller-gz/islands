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
#include <cartridge/cartridge.hpp>
#include <cartridge/requirements.internal.hpp>
#include <filesystem>
#include <logger.hpp>
#define LOGGER_CATEGORY "~/cartridge::requirements"

static auto probe(const String &bundle, Vector<String> &trail) -> Flag {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  for (const auto &seen : trail)
    if (seen == bundle) {
      logger.error(
        "Requirement cycle: %s recurs in its own requirement "
        "chain.",
        bundle.c_str());
      return true;
    }
  const String file = bundle + "/" + CARTRIDGE::MANIFEST::NAME;
  if (!std::filesystem::exists(file)) return false;
  CARTRIDGE::Manifest manifest;
  if (CARTRIDGE::MANIFEST::load(file, manifest)) return false;
  trail.push_back(bundle);
  for (const auto &requirement : manifest.requirements)
    if (probe(requirement.bundle, trail)) return true;
  trail.pop_back();
  return false;
}

auto REQUIREMENTS::cycles() -> Flag {
  Vector<String> trail = {CARTRIDGE::GET::bundle()};
  for (const auto &requirement : CARTRIDGE::requirements)
    if (probe(requirement.bundle, trail)) return true;
  return false;
}
