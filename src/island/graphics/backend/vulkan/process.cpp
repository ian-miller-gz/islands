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
#include <island.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#include <island/window/backend.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

Flag GFX::BACKEND::poll() {
  if (!WINDOW::poll()) {
    ISLAND::STATE::terminate = true;
    return false;
  }
  WINDOW::GET::size(&GFX::WINDOWS::MAIN::width, &GFX::WINDOWS::MAIN::height);
  return GFX::WINDOWS::MAIN::width != 0 && GFX::WINDOWS::MAIN::height != 0;
}

#endif
