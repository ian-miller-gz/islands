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

static auto locate() -> String {
  if (CARTRIDGE::bound->manifest)
    return String("./") + CARTRIDGE::bound->manifest;
#ifdef SR_CARTRIDGE_BUNDLE
  return String("./" SR_CARTRIDGE_BUNDLE "/") + CARTRIDGE::MANIFEST::NAME;
#else
  return "";
#endif
}

auto CARTRIDGE::bind(LOGGER::Category &logger) -> Status {
  logger.info("Binding the linked cartridge...");
  bound = &cartridge();
  const String path = locate();
  if (path.empty()) return 0;
  Manifest manifest;
  if (MANIFEST::load(path, manifest)) return 1;
  adopt(manifest, path.substr(0, path.rfind('/') + 1));
  return 0;
}

void CARTRIDGE::release() {}

auto CARTRIDGE::configured() -> Flag { return false; }

auto CARTRIDGE::GET::bundle() -> String {
  if (!bound) return NONE;
  if (!bound->manifest) {
#ifdef SR_CARTRIDGE_BUNDLE
    return SR_CARTRIDGE_BUNDLE;
#else
    return NONE;
#endif
  }
  const String file = bound->manifest;
  auto cut = file.rfind('/');
  return cut == String::npos ? String(".") : file.substr(0, cut);
}

#endif
