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
#include <island/graphics/backend/vulkan/resources.internal.hpp>
#define LOGGER_CATEGORY "~/island/graphics/vulkan::fields"

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

auto GFX::FIELDS::create(Whole width, Whole height) -> GFX::Handle {
  auto handle = adopt(FIELD::create(width, height), width, height, true);
  LOGGER::get(LOGGER_CATEGORY)
    .debug(
      "Field created: %zu x %zu", static_cast<size_t>(width),
      static_cast<size_t>(height));
  return handle;
}

void GFX::FIELDS::destroy(GFX::Handle field) { GFX::TEXTURES::destroy(field); }

void GFX::FIELDS::close() {}

#endif
