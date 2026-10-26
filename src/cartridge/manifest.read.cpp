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
#include <cartridge/manifest.hpp>
#include <cartridge/manifest.internal.hpp>
#include <filesystem>

auto CARTRIDGE::MANIFEST::read(const String &directory) -> Manifest {
  Manifest manifest;
  IO::STREAMS::Input file(directory + "/" + NAME);
  if (file) parse(file, manifest, false);
  return manifest;
}

auto CARTRIDGE::MANIFEST::entry(
  const String &directory, const Manifest &manifest) -> String {
  if (manifest.entry.empty()) return manifest.entry;
  const String flat = directory + "/" + manifest.entry;
  const String folded =
    directory + "/" + AXES::platform() + "/" + manifest.entry;
  std::error_code ec;
  return std::filesystem::exists(folded, ec) ? folded : flat;
}
