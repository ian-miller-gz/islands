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
#include <cartridge.hpp>
#include <island/graphics/backend/render.hpp>
#include <island/graphics/backend/vulkan/devices/internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#include <island/window/backend.hpp>
#define LOGGER_CATEGORY "~/island/graphics/vulkan::initialize"

#if SR_GFX_BACKEND == SR_VULKAN

void GFX::BACKEND::initialize() {
  auto &logger = LOGGER::get(LOGGER_CATEGORY);
  logger.info("Using Vulkan as graphics backend.");

  WINDOW::initialize();
  WINDOW::create(
    CARTRIDGE::GET::title().c_str(), GFX::WINDOWS::MAIN::width,
    GFX::WINDOWS::MAIN::height);
  WINDOW::GET::size(&GFX::WINDOWS::MAIN::width, &GFX::WINDOWS::MAIN::height);
  GFX::STATE::scale = WINDOW::GET::scale();
  logger.info(
    "Window created: %d x %d", GFX::WINDOWS::MAIN::width,
    GFX::WINDOWS::MAIN::height);

  VULKAN::instance.initialize();
  logger.debug("Instance, surface, and validation ready.");
  GFX::STATE::gpu.initialize();
  logger.info("Physical device selected");
  logger.debug(
    "Device: %s", GFX::STATE::gpu.physical.getProperties().deviceName.data());

  VULKAN::swapchain.initialize();
  logger.info(
    "Swap chain created in %s with %zu images",
    VULKAN::Swapchain::name(VULKAN::swapchain.mode),
    VULKAN::swapchain.images.size());
  logger.debug(
    "Swap chain: %u x %u, format %d", VULKAN::swapchain.extent.width,
    VULKAN::swapchain.extent.height,
    static_cast<int>(VULKAN::swapchain.format.format));

  VULKAN::pass = VULKAN::compose();
  VULKAN::swapchain.attach();
  logger.debug("Framebuffers attached to the render pass.");
  VULKAN::frames.initialize();
  logger.debug(
    "Frame ring ready: %u frames in flight.",
    static_cast<unsigned>(VULKAN::FRAMES));
  GFX::BACKEND::await();
  CARTRIDGE::initialize();

  logger.info("Vulkan backend initialized successfully");
}

Flag GFX::BACKEND::ready() { return true; }

#endif
