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
#include <island/graphics/backend/vulkan/state.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

void GFX::BACKEND::VULKAN::Frames::initialize() {
  auto &gpu = GFX::STATE::gpu;
  vk::CommandPoolCreateInfo info{
    .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
    .queueFamilyIndex = static_cast<uint32_t>(gpu.family)};
  pool = vk::raii::CommandPool(gpu.logical, info);
  vk::CommandBufferAllocateInfo allocation{
    .commandPool = *pool,
    .level = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = FRAMES};
  auto buffers = vk::raii::CommandBuffers(gpu.logical, allocation);
  for (Whole i = 0; i < FRAMES; i += 1) {
    ring.push_back(
      {.commands = std::move(buffers[i]),
       .ready = vk::raii::Semaphore(gpu.logical, vk::SemaphoreCreateInfo{}),
       .busy = vk::raii::Fence(
         gpu.logical, {.flags = vk::FenceCreateFlagBits::eSignaled})});
  }
}

void GFX::BACKEND::VULKAN::Frames::close() {
  for (auto &frame : ring) frame.retired.close();
  ring.clear();
  pool.clear();
  index = 0;
}

auto GFX::BACKEND::VULKAN::COMMANDS::create() -> vk::raii::CommandBuffers {
  vk::CommandBufferAllocateInfo info{
    .commandPool = *frames.pool,
    .level = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = 1};
  vk::raii::CommandBuffers buffers(GFX::STATE::gpu.logical, info);
  buffers.front().begin(
    {.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
  return buffers;
}

void GFX::BACKEND::VULKAN::flush(const vk::raii::CommandBuffer &commands) {
  commands.end();
  vk::SubmitInfo info{.commandBufferCount = 1, .pCommandBuffers = &(*commands)};
  GFX::STATE::gpu.queue.submit(info, nullptr);
  GFX::STATE::gpu.queue.waitIdle();
}

#endif
