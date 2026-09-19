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

namespace {
auto width(const String &line) -> Whole {
  const auto at = line.find_first_not_of(" \t");
  return at == String::npos ? 0 : at;
}
}  // namespace

auto CARTRIDGE::MANIFEST::fold(
  const String &line, Manifest &manifest, Fold &state) -> Status {
  String key, value;
  if (!STRING::pair(line, key, value)) return 0;
  const Whole indent = ::width(line);
  if (!state.names) state.names = indent;
  if (indent <= state.names) {
    state.mine = key == AXES::platform();
    state.keys = 0;
    state.engine = false;
    return 0;
  }
  if (!state.mine) return 0;
  if (!state.keys) state.keys = indent;
  if (indent > state.keys) return state.engine ? demand(line, manifest) : 0;
  if ((state.engine = key == SECTION)) return 0;
  if (reserved(key)) return assign(manifest, key, value);
  manifest.keys[key] = value;
  return 0;
}
