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
#include <utility>

#if SR_GFX_BACKEND == SR_VULKAN

namespace {
constexpr Whole CHUNK = 1 << 20;

struct Slot {
  GFX::Buffer<Byte> staging;
  Whole size = 0;
  Whole cursor = 0;
};

struct Patch {
  GFX::Handle texture = GFX::NONE;
  Whole offset = 0;
  Whole x = 0, y = 0, width = 0, height = 0;
  Flag fresh = false;
};
}  // namespace

static Slot slots[GFX::BACKEND::VULKAN::FRAMES];
static Vector<Patch> patches;

static auto room(Slot &slot, Whole size) -> Whole {
  if (slot.cursor + size > slot.size) {
    const auto wanted = ((slot.cursor + size + CHUNK - 1) / CHUNK) * CHUNK;
    auto grown = GFX::BACKEND::VULKAN::STAGING::create(wanted);
    if (slot.cursor) std::memcpy(grown.data, slot.staging.data, slot.cursor);
    slot.staging.close();
    slot.staging = std::move(grown);
    slot.size = wanted;
  }
  const auto offset = slot.cursor;
  slot.cursor += size;
  return offset;
}

static auto copy(const Patch &patch) -> vk::BufferImageCopy {
  return {
    .bufferOffset = patch.offset,
    .imageSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1},
    .imageOffset =
      {static_cast<int32_t>(patch.x), static_cast<int32_t>(patch.y), 0},
    .imageExtent = {
      static_cast<uint32_t>(patch.width), static_cast<uint32_t>(patch.height),
      1}};
}

void GFX::BACKEND::VULKAN::PATCHES::stage(
  GFX::Handle texture, const void *pixels, Whole x, Whole y, Whole width,
  Whole height, Whole depth, Flag fresh) {
  const auto size = width * height * depth;
  if (!size) return;
  auto &slot = ::slots[frames.index];
  const auto offset = ::room(slot, size);
  std::memcpy(slot.staging.data + offset, pixels, size);
  ::patches.push_back({texture, offset, x, y, width, height, fresh});
}

void GFX::BACKEND::VULKAN::PATCHES::record(
  const vk::raii::CommandBuffer &commands) {
  auto &slot = ::slots[frames.index];
  for (auto &patch : ::patches) {
    auto &texture = GET::texture(patch.texture);
    if (!*texture.image) continue;
    const auto from = patch.fresh ? vk::ImageLayout::eUndefined
                                  : vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier(
      commands, *texture.image, from, vk::ImageLayout::eTransferDstOptimal);
    commands.copyBufferToImage(
      *slot.staging.handle, *texture.image,
      vk::ImageLayout::eTransferDstOptimal, ::copy(patch));
    barrier(
      commands, *texture.image, vk::ImageLayout::eTransferDstOptimal,
      vk::ImageLayout::eShaderReadOnlyOptimal);
  }
  ::patches.clear();
  slot.cursor = 0;
}

void GFX::BACKEND::VULKAN::PATCHES::close() {
  ::patches.clear();
  for (auto &slot : ::slots) {
    slot.staging.close();
    slot.size = slot.cursor = 0;
  }
}

#endif
