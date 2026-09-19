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
#include <island/graphics/backend/vulkan/devices/internal.hpp>
#include <island/graphics/backend/vulkan/render.internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#define LOGGER_CATEGORY "~/island/graphics/vulkan::swapchain"

#if SR_GFX_BACKEND == SR_VULKAN

auto GFX::BACKEND::VULKAN::Swapchain::resize() -> Flag {
  const auto width = GFX::WINDOWS::MAIN::width;
  const auto height = GFX::WINDOWS::MAIN::height;
  if (width == 0 || height == 0) return false;
  if (vsync != GFX::VSYNC::on) {
    recreate("vsync toggled");
  } else if (
    static_cast<uint32_t>(width) != extent.width ||
    static_cast<uint32_t>(height) != extent.height) {
    recreate("window size changed");
  }
  return true;
}

void GFX::BACKEND::VULKAN::Swapchain::recreate(STRING::Hot reason) {
  auto previous = extent;
  GFX::STATE::gpu.logical.waitIdle();
  close();
  initialize();
  attach();
  VULKAN::drop();
  LOGGER::get(LOGGER_CATEGORY)
    .info(
      "Swap chain recreated (%s): %u x %u -> %u x %u (%s, %zu images)", reason,
      previous.width, previous.height, extent.width, extent.height, name(mode),
      images.size());
}

#endif
