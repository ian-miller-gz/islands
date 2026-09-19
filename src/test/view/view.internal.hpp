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
#include <island/graphics/backend/resources.hpp>
#include <island/graphics/viewports.hpp>

namespace VIEW {
constexpr Whole WIDE = 320;
constexpr Whole TALL = 180;

auto sampled() -> GFX::SAMPLERS::Kind;

auto check(const String &name, Flag ok) -> Status;
void window(int width, int height);
auto matches(Float value, Float expected) -> Flag;
auto fits(const GFX::Viewport &rect) -> Flag;

auto integer() -> Status;
auto letterbox() -> Status;
}  // namespace VIEW
