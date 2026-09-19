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
#include <island/graphics/backend/vulkan/resources.internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

void GFX::BACKEND::VULKAN::Swapchain::initialize() {
  auto &gpu = GFX::STATE::gpu;
  auto capabilities = gpu.physical.getSurfaceCapabilitiesKHR(*instance.surface);
  format = select(gpu.physical.getSurfaceFormatsKHR(*instance.surface));
  extent = measure(capabilities);
  mode = select(gpu.physical.getSurfacePresentModesKHR(*instance.surface));
  vsync = GFX::VSYNC::on;
  handle = vk::raii::SwapchainKHR(
    gpu.logical, describe(capabilities, format, extent, mode));
  images = handle.getImages();
  for (auto &image : images) {
    vk::ImageViewCreateInfo info{
      .image = image,
      .viewType = vk::ImageViewType::e2D,
      .format = format.format,
      .subresourceRange = {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1}};
    views.emplace_back(gpu.logical, info);
    semaphores.emplace_back(gpu.logical, vk::SemaphoreCreateInfo{});
  }
  depth = VULKAN::DEPTH::create(extent.width, extent.height);
}

void GFX::BACKEND::VULKAN::Swapchain::attach() {
  for (auto &view : views) {
    vk::ImageView attachments[] = {*view, *depth.view};
    vk::FramebufferCreateInfo info{
      .renderPass = *pass,
      .attachmentCount = 2,
      .pAttachments = attachments,
      .width = extent.width,
      .height = extent.height,
      .layers = 1};
    targets.emplace_back(GFX::STATE::gpu.logical, info);
  }
}

void GFX::BACKEND::VULKAN::Swapchain::close() {
  targets.clear();
  depth.close();
  semaphores.clear();
  views.clear();
  images.clear();
  handle.clear();
}

#endif
