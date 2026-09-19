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
#include <island/graphics/backend/vulkan/state.hpp>
#include <cstring>

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

static auto stride(Whole size) -> Whole {
  static const auto ALIGN =
    static_cast<Whole>(GFX::STATE::gpu.physical.getProperties()
                         .limits.minUniformBufferOffsetAlignment);
  return (size + ALIGN - 1) / ALIGN * ALIGN;
}

static void grow(Pipeline &entry, Whole regions) {
  GFX::STATE::gpu.logical.waitIdle();
  for (Whole slot = 0; slot < entry.uniforms.size(); slot += 1) {
    auto size = entry.descriptors.sizes[slot];
    auto backing = UNIFORM::create(stride(size) * (regions - 1) + size);
    std::memcpy(backing.data, entry.uniforms[slot].data, size);
    entry.uniforms[slot].close();
    entry.uniforms[slot] = std::move(backing);
  }
  entry.capacity = regions;
  entry.descriptors.write(entry.description, entry.uniforms);
}

static Vector<GFX::Handle> tallied;

void GFX::BACKEND::VULKAN::prepare() {
  tallied.clear();
  for (auto &pass : GFX::PASSES::list())
    for (auto &draw : pass.draws) {
      if (draw.uniforms.empty()) continue;
      auto &entry = GET::pipeline(draw.pipeline);
      if (!entry.counted) tallied.push_back(draw.pipeline);
      entry.counted += 1;
    }
  for (auto handle : tallied) {
    auto &entry = GET::pipeline(handle);
    if (entry.counted + 1 > entry.capacity) grow(entry, entry.counted + 1);
    entry.cursor = 0;
    entry.counted = 0;
  }
}

auto GFX::BACKEND::VULKAN::feed(Pipeline &entry, const GFX::Draw &draw)
  -> const Vector<uint32_t> & {
  auto &offsets = entry.offsets;
  offsets.assign(entry.uniforms.size(), 0);
  if (draw.uniforms.empty()) return offsets;
  entry.cursor += 1;
  for (Whole slot = 0; slot < offsets.size(); slot += 1) {
    if (slot >= draw.uniforms.size() || draw.uniforms[slot].empty()) continue;
    auto &block = draw.uniforms[slot];
    auto offset = stride(entry.descriptors.sizes[slot]) * entry.cursor;
    std::memcpy(entry.uniforms[slot].data + offset, block.data(), block.size());
    offsets[slot] = static_cast<uint32_t>(offset);
  }
  return offsets;
}

#endif
