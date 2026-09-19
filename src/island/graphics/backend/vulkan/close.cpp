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
#include <island/graphics/backend/vulkan/devices/internal.hpp>
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/backend/resources.hpp>
#include <island/window/backend.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

void GFX::BACKEND::reset() {
  auto &gpu = GFX::STATE::gpu;
  if (*gpu.logical) gpu.logical.waitIdle();
  GFX::PIPELINES::close();
  GFX::PIPELINES::FAMILIES::close();
  GFX::COMPUTE::close();
  GFX::SAMPLERS::close();
  GFX::TARGETS::close();
  GFX::FIELDS::close();
  GFX::TEXTURES::close();
  GFX::BUFFERS::close();
}

void GFX::BACKEND::close() {
  reset();
  VULKAN::pass.clear();
  VULKAN::frames.close();
  VULKAN::swapchain.close();
  GFX::STATE::gpu.close();
  VULKAN::instance.close();

  WINDOW::destroy();
}

#endif
