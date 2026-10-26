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
#include <cartridge.hpp>
#include <cartridge/load.internal.hpp>
#include <logger.hpp>
#include <string>
#if SR_CARTRIDGE != SR_NONE

// The reef host's static-delivery arm. A statics: fold with `host: Reef`
// links a serving cartridge into this binary and names it SR_REEF_BUNDLE
// (the launcher's monitor on Windows); the arm then binds the linked
// entry the way the island host binds its own. Without such a fold the
// arm refuses, as it always did.
#ifdef SR_REEF_BUNDLE

static auto locate() -> String {
  if (CARTRIDGE::bound->manifest)
    return String("./") + CARTRIDGE::bound->manifest;
  return String("./" SR_REEF_BUNDLE "/") + CARTRIDGE::MANIFEST::NAME;
}

auto CARTRIDGE::bind(LOGGER::Category &logger) -> Status {
  logger.info("Binding the linked cartridge...");
  bound = &cartridge();
  const String path = locate();
  Manifest manifest;
  if (MANIFEST::load(path, manifest)) return 1;
  adopt(manifest, path.substr(0, path.rfind('/') + 1));
  return 0;
}

auto CARTRIDGE::configured() -> Flag { return true; }

auto CARTRIDGE::GET::bundle() -> String {
  if (!bound || !bound->manifest) return SR_REEF_BUNDLE;
  const String file = bound->manifest;
  auto cut = file.rfind('/');
  return cut == String::npos ? String(".") : file.substr(0, cut);
}

#else

auto CARTRIDGE::bind(LOGGER::Category &logger) -> Status {
  logger.error("Static delivery folds no cartridge into this host.");
  return 1;
}

auto CARTRIDGE::configured() -> Flag { return false; }

auto CARTRIDGE::GET::bundle() -> String { return NONE; }

#endif

void CARTRIDGE::release() {}

#endif
