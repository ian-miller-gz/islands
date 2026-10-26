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
#include <cartridge/browse.internal.hpp>

#include <algorithm>
#include <cstdlib>
#include <string_view>

namespace CARTRIDGE::BROWSE {
namespace {

auto addressed(STRING::Hot url) -> Flag {
  if (url == nullptr) return false;
  const std::string_view address = url;
  return std::ranges::any_of(SCHEMES, [&](std::string_view scheme) {
    return address.size() > scheme.size() && address.starts_with(scheme);
  });
}

auto chosen() -> STRING::Hot {
  const STRING::Hot program = std::getenv(VARIABLE);
  if (program == nullptr || std::string_view(program).empty()) return nullptr;
  return program;
}

}  // namespace
}  // namespace CARTRIDGE::BROWSE

auto CARTRIDGE::browse(STRING::Hot url) -> Status {
  if (!BROWSE::addressed(url)) return BROWSE::ADDRESS;
  const STRING::Hot program = BROWSE::chosen();
  if (program != nullptr && BROWSE::open(program, url)) return 0;
  return BROWSE::open(url) ? 0 : BROWSE::OPENER;
}
