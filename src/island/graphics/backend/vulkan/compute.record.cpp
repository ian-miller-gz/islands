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
#include <island/graphics/backend/vulkan/compute.hpp>
#include <island/graphics/backend/vulkan/pipelines.hpp>
#include <island/graphics/backend/vulkan/resources.internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#include <cstring>

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;
using enum vk::DescriptorType;

static void realize(Kernel &kernel, const GFX::Bindings &bindings) {
  if (kernel.primed && kernel.bound == bindings) return;
  GFX::STATE::gpu.logical.waitIdle();
  const auto &stage = kernel.description.stage;
  Vector<vk::DescriptorImageInfo> images;
  images.reserve(bindings.samplers.size() + bindings.images.size());
  Vector<vk::DescriptorBufferInfo> buffers;
  buffers.reserve(bindings.buffers.size());
  Vector<vk::WriteDescriptorSet> writes;
  for (Whole slot = 0; slot < bindings.samplers.size(); slot += 1) {
    auto &sampling = bindings.samplers[slot];
    images.push_back(
      {.sampler = *GET::sampler(sampling.sampler),
       .imageView = *GET::texture(sampling.texture).view,
       .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal});
    writes.push_back(
      write(kernel.sets[0], slot, eCombinedImageSampler, images.back()));
  }
  for (Whole slot = 0; slot < bindings.buffers.size(); slot += 1) {
    buffers.push_back(
      {*GET::buffer(bindings.buffers[slot]).handle, 0, VK_WHOLE_SIZE});
    writes.push_back(write(
      kernel.sets[0], stage.samplers + slot, eStorageBuffer, buffers.back()));
  }
  for (Whole slot = 0; slot < bindings.images.size(); slot += 1) {
    images.push_back(
      {.imageView = *GET::texture(bindings.images[slot]).view,
       .imageLayout = vk::ImageLayout::eGeneral});
    writes.push_back(write(kernel.sets[1], slot, eStorageImage, images.back()));
  }
  GFX::STATE::gpu.logical.updateDescriptorSets(writes, {});
  kernel.bound = bindings;
  kernel.primed = true;
}

static void overrides(Kernel &kernel, const Vector<Vector<Byte>> &blocks) {
  const auto &sizes = kernel.description.stage.uniforms;
  for (Whole slot = 0; slot < blocks.size() && slot < kernel.uniforms.size();
       slot += 1)
    if (!blocks[slot].empty())
      std::memcpy(kernel.uniforms[slot].data, blocks[slot].data(), sizes[slot]);
}

void GFX::BACKEND::VULKAN::compute(
  const vk::raii::CommandBuffer &commands, const GFX::Dispatch &dispatch) {
  auto &kernel = GET::kernel(dispatch.pipeline);
  if (!*kernel.handle) return;
  realize(kernel, dispatch.bindings);
  overrides(kernel, dispatch.uniforms);
  commands.bindPipeline(vk::PipelineBindPoint::eCompute, *kernel.handle);
  Vector<vk::DescriptorSet> handles;
  for (auto &set : kernel.sets) handles.push_back(*set);
  commands.bindDescriptorSets(
    vk::PipelineBindPoint::eCompute, *kernel.layout, 0, handles, {});
  commands.dispatch(
    static_cast<uint32_t>(dispatch.groups.x),
    static_cast<uint32_t>(dispatch.groups.y),
    static_cast<uint32_t>(dispatch.groups.z));
}

void GFX::BACKEND::VULKAN::boundary(const vk::raii::CommandBuffer &commands) {
  vk::MemoryBarrier barrier{
    .srcAccessMask = vk::AccessFlagBits::eShaderWrite,
    .dstAccessMask =
      vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite};
  commands.pipelineBarrier(
    vk::PipelineStageFlagBits::eComputeShader,
    vk::PipelineStageFlagBits::eComputeShader |
      vk::PipelineStageFlagBits::eVertexShader |
      vk::PipelineStageFlagBits::eFragmentShader,
    {}, barrier, {}, {});
}

#endif
