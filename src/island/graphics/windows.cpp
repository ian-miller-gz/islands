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
#include <island/graphics/windows.hpp>

GFX::WINDOWS::Mode GFX::WINDOWS::MAIN::mode = GFX::WINDOWS::Mode::FULL;
int GFX::WINDOWS::MAIN::width = 0;
int GFX::WINDOWS::MAIN::height = 0;

auto GFX::WINDOWS::MAIN::viewport() -> GFX::Viewport {
  return {0.0f, 0.0f, static_cast<Float>(width), static_cast<Float>(height)};
}