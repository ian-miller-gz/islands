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
#include <island/run.hpp>
#include <cartridge.hpp>
#include <common.hpp>
#include <generated/engine.hpp>
#include <filesystem>
#include <iostream>
#include <unistd.h>

#if SR_CARTRIDGE == SR_NONE
namespace ISLAND::RUN {
namespace {
constexpr STRING::Hot LAUNCHER = "cartridges/.core/launcher";
constexpr STRING::Hot ISLAND = "island";
constexpr STRING::Hot REEF = "reef";

auto seat(const String &bundle) -> String {
  const String file = String("/") + CARTRIDGE::MANIFEST::NAME;
  std::error_code ec;
  if (std::filesystem::exists(bundle + file, ec)) return bundle;
  return COMMON::HOME + "/" + bundle;
}

void hand(char **values) {
  const String binary =
    COMMON::BINARY + "/" + COMMON::PLATFORM::binary(REEF_NAME);
  execv(binary.c_str(), values);
  std::cout << "Cannot run " << binary << std::endl;
}
}  // namespace

auto dispatch(char **values) -> Status {
  if (CARTRIDGE::path.empty()) CARTRIDGE::path = LAUNCHER;
  if (CARTRIDGE::path == CARTRIDGE::NONE) return 0;
  const String word = CARTRIDGE::MANIFEST::read(seat(CARTRIDGE::path)).host;
  if (word.empty() || word == ISLAND) return 0;
  if (word == REEF) {
    hand(values);
    return 1;
  }
  std::cout << "Unknown host in " << CARTRIDGE::path << "/"
            << CARTRIDGE::MANIFEST::NAME << " (island or reef)" << std::endl;
  return 1;
}
}  // namespace ISLAND::RUN
#else
auto ISLAND::RUN::dispatch(char **) -> Status { return 0; }
#endif
