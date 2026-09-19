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

#include <island/graphics/backend/passes.hpp>
#include <island/graphics/color.hpp>
#include <console.hpp>
#include <common.hpp>

namespace TERMINAL {

constexpr GFX::Color BACKGROUND = {0.76f, 0.83f, 0.91f, 0.95f};

constexpr GFX::Color PROMPT = {0.12f, 0.15f, 0.20f, 1.0f};

auto band(const CONSOLES::Console &console, const GFX::Pass &pass) -> Float;

auto scale() -> Float;

auto line() -> Float;

auto rows(const CONSOLES::Console &console, const GFX::Pass &pass, Float line)
  -> Whole;

void fill(GFX::Viewport &viewport);

void background();

}  // namespace TERMINAL
