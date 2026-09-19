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
#include <island/graphics/backend/vulkan/state.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;
using enum vk::DescriptorType;
constexpr auto COMPUTE = vk::ShaderStageFlagBits::eCompute;

static void append(
  Vector<vk::DescriptorSetLayoutBinding> &bindings, Whole count,
  vk::DescriptorType type) {
  for (Whole i = 0; i < count; i += 1)
    bindings.push_back(
      {.binding = static_cast<uint32_t>(bindings.size()),
       .descriptorType = type,
       .descriptorCount = 1,
       .stageFlags = COMPUTE});
}

static auto shape(const Vector<vk::DescriptorSetLayoutBinding> &bindings)
  -> vk::raii::DescriptorSetLayout {
  vk::DescriptorSetLayoutCreateInfo info{
    .bindingCount = static_cast<uint32_t>(bindings.size()),
    .pBindings = bindings.data()};
  return vk::raii::DescriptorSetLayout(GFX::STATE::gpu.logical, info);
}

void GFX::BACKEND::VULKAN::reserve(Kernel &kernel) {
  const auto &stage = kernel.description.stage;
  Vector<vk::DescriptorSetLayoutBinding> inputs, images, blocks;
  append(inputs, stage.samplers, eCombinedImageSampler);
  append(inputs, stage.buffers, eStorageBuffer);
  append(images, stage.images, eStorageImage);
  append(blocks, stage.uniforms.size(), eUniformBuffer);
  for (auto *set : {&inputs, &images, &blocks})
    kernel.layouts.push_back(shape(*set));
  Vector<vk::DescriptorPoolSize> sizes;
  auto add = [&sizes](vk::DescriptorType type, Whole count) {
    if (count) sizes.push_back({type, static_cast<uint32_t>(count)});
  };
  add(eCombinedImageSampler, stage.samplers);
  add(eStorageBuffer, stage.buffers);
  add(eStorageImage, stage.images);
  add(eUniformBuffer, stage.uniforms.size());
  vk::DescriptorPoolCreateInfo info{
    .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
    .maxSets = 3,
    .poolSizeCount = static_cast<uint32_t>(sizes.size()),
    .pPoolSizes = sizes.data()};
  kernel.pool = vk::raii::DescriptorPool(GFX::STATE::gpu.logical, info);
  Vector<vk::DescriptorSetLayout> handles;
  for (auto &layout : kernel.layouts) handles.push_back(*layout);
  vk::DescriptorSetAllocateInfo request{
    .descriptorPool = *kernel.pool,
    .descriptorSetCount = 3,
    .pSetLayouts = handles.data()};
  for (auto &set : vk::raii::DescriptorSets(GFX::STATE::gpu.logical, request))
    kernel.sets.push_back(std::move(set));
}

void GFX::BACKEND::VULKAN::point(Kernel &kernel) {
  const auto &sizes = kernel.description.stage.uniforms;
  Vector<vk::DescriptorBufferInfo> infos;
  infos.reserve(kernel.uniforms.size());
  Vector<vk::WriteDescriptorSet> writes;
  for (Whole slot = 0; slot < kernel.uniforms.size(); slot += 1) {
    infos.push_back({*kernel.uniforms[slot].handle, 0, sizes[slot]});
    writes.push_back(write(kernel.sets[2], slot, eUniformBuffer, infos.back()));
  }
  if (!writes.empty()) GFX::STATE::gpu.logical.updateDescriptorSets(writes, {});
}

#endif
