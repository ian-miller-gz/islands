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

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

static void resources(
  const vk::raii::DescriptorSet &set, const GFX::Stage &stage,
  const GFX::Bindings &bindings, Vector<vk::DescriptorBufferInfo> &buffers,
  Vector<vk::DescriptorImageInfo> &images,
  Vector<vk::WriteDescriptorSet> &writes) {
  constexpr auto STORAGE = vk::DescriptorType::eStorageBuffer;
  constexpr auto COMBINED = vk::DescriptorType::eCombinedImageSampler;
  for (Whole slot = 0; slot < bindings.samplers.size(); slot += 1) {
    auto &sampling = bindings.samplers[slot];
    images.push_back(
      {.sampler = *GET::sampler(sampling.sampler),
       .imageView = *GET::texture(sampling.texture).view,
       .imageLayout = GET::storage(sampling.texture)
                        ? vk::ImageLayout::eGeneral
                        : vk::ImageLayout::eShaderReadOnlyOptimal});
    writes.push_back(write(set, slot, COMBINED, images.back()));
  }
  Whole base = stage.samplers + stage.images;
  for (Whole slot = 0; slot < bindings.buffers.size(); slot += 1) {
    buffers.push_back(
      {*GET::buffer(bindings.buffers[slot]).handle, 0, VK_WHOLE_SIZE});
    writes.push_back(write(set, base + slot, STORAGE, buffers.back()));
  }
}

void GFX::BACKEND::VULKAN::rebind(Pipeline &entry, const GFX::Draw &draw) {
  if (
    entry.bound.vertex == draw.vertex &&
    entry.bound.fragment == draw.fragment) {
    return;
  }
  GFX::STATE::gpu.logical.waitIdle();
  Vector<vk::DescriptorBufferInfo> buffers;
  buffers.reserve(draw.vertex.buffers.size() + draw.fragment.buffers.size());
  Vector<vk::DescriptorImageInfo> images;
  images.reserve(draw.vertex.samplers.size() + draw.fragment.samplers.size());
  Vector<vk::WriteDescriptorSet> writes;
  resources(
    entry.descriptors.sets[0], entry.description.vertex, draw.vertex, buffers,
    images, writes);
  resources(
    entry.descriptors.sets[2], entry.description.fragment, draw.fragment,
    buffers, images, writes);
  GFX::STATE::gpu.logical.updateDescriptorSets(writes, {});
  entry.bound = draw;
}

#endif
