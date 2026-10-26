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

#include <common.hpp>

namespace CARTRIDGE {
auto browse(STRING::Hot url) -> Status;
}  // namespace CARTRIDGE

namespace CARTRIDGE::BROWSE {
constexpr Status ADDRESS = 1;
constexpr Status OPENER = 2;
constexpr STRING::Hot VARIABLE = "BROWSER";
constexpr STRING::Hot SCHEMES[] = {"http://", "https://"};
constexpr STRING::Hot REFUSALS[] = {"", "wrong scheme", "no opener"};
}  // namespace CARTRIDGE::BROWSE
