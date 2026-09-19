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
#define LOGGER_CATEGORY "~/island/graphics/vulkan::textures"

#if SR_GFX_BACKEND == SR_VULKAN

void GFX::BACKEND::VULKAN::back(GFX::Texture &texture) {
  auto needs = texture.image.getMemoryRequirements();
  vk::MemoryAllocateInfo allocation{
    .allocationSize = needs.size,
    .memoryTypeIndex =
      GET::memory(needs, vk::MemoryPropertyFlagBits::eDeviceLocal)};
  texture.memory = vk::raii::DeviceMemory(GFX::STATE::gpu.logical, allocation);
  texture.image.bindMemory(*texture.memory, 0);
}

auto GFX::BACKEND::VULKAN::IMAGE::create(
  Whole width, Whole height, vk::Format format) -> GFX::Texture {
  GFX::Texture texture;
  vk::ImageCreateInfo info{
    .imageType = vk::ImageType::e2D,
    .format = format,
    .extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1},
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = vk::SampleCountFlagBits::e1,
    .usage =
      vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled};
  texture.image = vk::raii::Image(GFX::STATE::gpu.logical, info);
  back(texture);
  vk::ImageViewCreateInfo view{
    .image = *texture.image,
    .viewType = vk::ImageViewType::e2D,
    .format = format,
    .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};
  texture.view = vk::raii::ImageView(GFX::STATE::gpu.logical, view);
  LOGGER::get(LOGGER_CATEGORY)
    .debug(
      "Texture created: %zu x %zu", static_cast<size_t>(width),
      static_cast<size_t>(height));
  return texture;
}

#endif
