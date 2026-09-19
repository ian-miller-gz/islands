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

#if SR_GFX_BACKEND == SR_VULKAN

auto GFX::BACKEND::VULKAN::DEPTH::create(Whole width, Whole height)
  -> GFX::Texture {
  GFX::Texture texture;
  vk::ImageCreateInfo info{
    .imageType = vk::ImageType::e2D,
    .format = FORMAT,
    .extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1},
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = vk::SampleCountFlagBits::e1,
    .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment};
  texture.image = vk::raii::Image(GFX::STATE::gpu.logical, info);
  back(texture);
  vk::ImageViewCreateInfo view{
    .image = *texture.image,
    .viewType = vk::ImageViewType::e2D,
    .format = FORMAT,
    .subresourceRange = {vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1}};
  texture.view = vk::raii::ImageView(GFX::STATE::gpu.logical, view);
  return texture;
}

#endif
