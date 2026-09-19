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
#include <cartridge/load.internal.hpp>
#include <cartridge/manifest.hpp>
#include <cartridge/manifest.internal.hpp>
#include <logger.hpp>
#include <string>
#if SR_CARTRIDGE == SR_NONE

String CARTRIDGE::path;

auto CARTRIDGE::bind(LOGGER::Category &logger) -> Status {
  if (path == NONE) {
    logger.info("Running without a cartridge.");
    return 0;
  }
  if (path.empty()) {
    logger.info("No cartridge configured.");
    return 0;
  }
  logger.info("Loading " + path + "...");
  Manifest manifest;
  if (MANIFEST::load(path + "/" + MANIFEST::NAME, manifest)) return 1;
  if (MANIFEST::vet(manifest, logger)) return 1;
  if (!(bound = open(MANIFEST::entry(path, manifest), logger))) return 1;
  adopt(manifest, path + "/");
  return 0;
}

auto CARTRIDGE::configured() -> Flag { return !path.empty() && path != NONE; }

auto CARTRIDGE::GET::bundle() -> String { return path.empty() ? NONE : path; }

#endif
