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
#include <island/graphics/backend/vulkan/state.hpp>
#include <stdexcept>

#if SR_GFX_BACKEND == SR_VULKAN

auto GFX::BACKEND::VULKAN::GET::memory(
  const vk::MemoryRequirements &needs,
  vk::MemoryPropertyFlags wanted) -> uint32_t {
  auto properties = GFX::STATE::gpu.physical.getMemoryProperties();
  for (uint32_t i = 0; i < properties.memoryTypeCount; i += 1) {
    if (
      (needs.memoryTypeBits & (1 << i)) &&
      (properties.memoryTypes[i].propertyFlags & wanted) == wanted) {
      return i;
    }
  }
  throw std::runtime_error("no compatible memory type");
}

#endif
