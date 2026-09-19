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
#pragma once
#include <cartridge/manifest.hpp>
#include <common.hpp>
#include <logger.hpp>

namespace CARTRIDGE::MANIFEST {
constexpr STRING::Hot SECTION = "engine";
constexpr STRING::Hot FOLDS = "platforms";

auto parse(IO::STREAMS::Input &file, Manifest &manifest, Flag folded) -> Status;

struct Fold {
  Whole names = 0;
  Whole keys = 0;
  Flag mine = false;
  Flag engine = false;
};

auto fold(const String &line, Manifest &manifest, Fold &state) -> Status;
auto require(const String &value, Manifest &manifest) -> Status;
auto assign(Manifest &manifest, const String &key, const String &value)
  -> Status;
auto reserved(const String &key) -> Flag;
auto window(const String &line, Manifest &manifest) -> Status;
auto designate(const String &line, Manifest &manifest) -> Status;
auto demand(const String &line, Manifest &manifest) -> Status;
auto vet(const Manifest &manifest, LOGGER::Category &logger) -> Status;
}  // namespace CARTRIDGE::MANIFEST
