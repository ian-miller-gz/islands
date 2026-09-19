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
#include <island/graphics/backend/vulkan/pipelines.hpp>
#include <island/graphics/backend/vulkan/resources.internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#define LOGGER_CATEGORY "~/island/graphics/vulkan::buffers"

#if SR_GFX_BACKEND == SR_VULKAN

template <typename Type>
static void bind(
  GFX::Buffer<Type> &buffer, Whole count, vk::BufferUsageFlags usage) {
  auto &gpu = GFX::STATE::gpu;
  vk::BufferCreateInfo info{
    .size = sizeof(Type) * count,
    .usage = usage,
    .sharingMode = vk::SharingMode::eExclusive};
  buffer.handle = vk::raii::Buffer(gpu.logical, info);
  auto needs = buffer.handle.getMemoryRequirements();
  auto wanted = vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent;
  vk::MemoryAllocateInfo allocation{
    .allocationSize = needs.size,
    .memoryTypeIndex = GFX::BACKEND::VULKAN::GET::memory(needs, wanted)};
  buffer.memory = vk::raii::DeviceMemory(gpu.logical, allocation);
  buffer.handle.bindMemory(*buffer.memory, 0);
  buffer.data = static_cast<Type *>(buffer.memory.mapMemory(0, VK_WHOLE_SIZE));
  LOGGER::get(LOGGER_CATEGORY)
    .debug(
      "Buffer created: %zu bytes, usage %u", static_cast<size_t>(info.size),
      static_cast<uint32_t>(usage));
}

auto GFX::BACKEND::VULKAN::UNIFORM::create(Whole size) -> GFX::Buffer<Byte> {
  GFX::Buffer<Byte> buffer;
  bind(buffer, size, vk::BufferUsageFlagBits::eUniformBuffer);
  return buffer;
}

static auto usage(GFX::BUFFERS::Kind kind) -> vk::BufferUsageFlags {
  using enum vk::BufferUsageFlagBits;
  if (kind == GFX::BUFFERS::VERTICES) return eVertexBuffer;
  if (kind == GFX::BUFFERS::INDICES) return eIndexBuffer;
  return eStorageBuffer;
}

auto GFX::BACKEND::VULKAN::MAPPED::create(Whole size, GFX::BUFFERS::Kind kind)
  -> GFX::Buffer<Byte> {
  GFX::Buffer<Byte> buffer;
  bind(buffer, size, usage(kind));
  return buffer;
}

auto GFX::BACKEND::VULKAN::READBACK::create(Whole size) -> GFX::Buffer<Byte> {
  GFX::Buffer<Byte> buffer;
  bind(buffer, size, vk::BufferUsageFlagBits::eTransferDst);
  return buffer;
}

auto GFX::BACKEND::VULKAN::STAGING::create(Whole size) -> GFX::Buffer<Byte> {
  GFX::Buffer<Byte> buffer;
  bind(buffer, size, vk::BufferUsageFlagBits::eTransferSrc);
  return buffer;
}

#endif
