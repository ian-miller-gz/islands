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
#include <cstring>

#if SR_GFX_BACKEND == SR_VULKAN

void GFX::BACKEND::VULKAN::barrier(
  const vk::raii::CommandBuffer &commands, vk::Image image,
  vk::ImageLayout from, vk::ImageLayout to) {
  vk::ImageMemoryBarrier info{
    .srcAccessMask = vk::AccessFlagBits::eMemoryWrite,
    .dstAccessMask =
      vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite,
    .oldLayout = from,
    .newLayout = to,
    .image = image,
    .subresourceRange = {
      .aspectMask = vk::ImageAspectFlagBits::eColor,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1}};
  commands.pipelineBarrier(
    vk::PipelineStageFlagBits::eAllCommands,
    vk::PipelineStageFlagBits::eAllCommands, {}, {}, {}, info);
}

void GFX::BACKEND::VULKAN::upload(
  GFX::Texture &texture, const void *pixels, Whole width, Whole height,
  Whole depth) {
  auto size = width * height * depth;
  auto buffer = STAGING::create(size);
  std::memcpy(buffer.data, pixels, size);
  auto recording = COMMANDS::create();
  auto &recorder = recording.front();
  barrier(
    recorder, *texture.image, vk::ImageLayout::eUndefined,
    vk::ImageLayout::eTransferDstOptimal);
  vk::BufferImageCopy region{
    .imageSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1},
    .imageExtent = {
      static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1}};
  recorder.copyBufferToImage(
    *buffer.handle, *texture.image, vk::ImageLayout::eTransferDstOptimal,
    region);
  barrier(
    recorder, *texture.image, vk::ImageLayout::eTransferDstOptimal,
    vk::ImageLayout::eShaderReadOnlyOptimal);
  flush(recorder);
  buffer.close();
}

#endif
