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
#include <island/window/backend/window.hpp>

#include <cstdint>

namespace WINDOW::NOTICE {
using Id = std::uint32_t;

constexpr Id NONE = 0;

constexpr Status SHOWN = 0;

auto show(STRING::Hot title, Id standing, Id &shown) -> Status;

void close(Id id);
}  // namespace WINDOW::NOTICE

namespace WINDOW::NOTICE::STATE {
extern Id standing;
}  // namespace WINDOW::NOTICE::STATE
