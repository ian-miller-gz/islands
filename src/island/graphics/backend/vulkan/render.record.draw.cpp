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
#include <island/graphics/backend/vulkan/render.internal.hpp>
#include <island/graphics/backend/vulkan/resources.internal.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

namespace {
struct Bound {
  GFX::Handle pipeline = GFX::NONE;
  Vector<uint32_t> offsets;
};
Bound bound;
}  // namespace

void GFX::BACKEND::VULKAN::unbind() { bound.pipeline = GFX::NONE; }

static void bind(
  const vk::raii::CommandBuffer &commands, const GFX::Draw &draw,
  const Pipeline &entry, const Vector<uint32_t> &offsets) {
  if (bound.pipeline == draw.pipeline && bound.offsets == offsets) return;
  commands.bindPipeline(vk::PipelineBindPoint::eGraphics, *entry.handle);
  commands.bindDescriptorSets(
    vk::PipelineBindPoint::eGraphics, *entry.layout, 0,
    entry.descriptors.handles, offsets);
  bound.pipeline = draw.pipeline;
  bound.offsets.assign(offsets.begin(), offsets.end());
}

void GFX::BACKEND::VULKAN::draw(
  const vk::raii::CommandBuffer &commands, const GFX::Draw &draw) {
  auto &entry = GET::pipeline(draw.pipeline);
  if (!*entry.handle) return;
  rebind(entry, draw);
  ::bind(commands, draw, entry, feed(entry, draw));
  if (draw.vertices != GFX::NONE) {
    commands.bindVertexBuffers(
      0, {*GET::buffer(draw.vertices).handle}, {vk::DeviceSize{0}});
  }
  if (draw.indices != GFX::NONE) {
    commands.bindIndexBuffer(
      *GET::buffer(draw.indices).handle, 0, vk::IndexType::eUint32);
    commands.drawIndexed(
      static_cast<uint32_t>(draw.count), 1, static_cast<uint32_t>(draw.first),
      0, 0);
    return;
  }
  commands.draw(
    static_cast<uint32_t>(draw.count), 1, static_cast<uint32_t>(draw.first), 0);
}

#endif
