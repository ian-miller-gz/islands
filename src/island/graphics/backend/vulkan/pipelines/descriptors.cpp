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
#include <island/graphics/backend/vulkan/state.hpp>
#include <island/graphics/backend/vulkan/pipelines.hpp>
#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;
using enum vk::DescriptorType;

static void append(
  Vector<vk::DescriptorSetLayoutBinding> &bindings, Whole count,
  vk::DescriptorType type, vk::ShaderStageFlags stages) {
  for (Whole i = 0; i < count; i += 1) {
    bindings.push_back(
      {.binding = static_cast<uint32_t>(bindings.size()),
       .descriptorType = type,
       .descriptorCount = 1,
       .stageFlags = stages});
  }
}

static void space(
  Vector<vk::raii::DescriptorSetLayout> &layouts, const GFX::Stage &stage,
  vk::ShaderStageFlags flags) {
  Vector<vk::DescriptorSetLayoutBinding> bindings;
  append(bindings, stage.samplers, eCombinedImageSampler, flags);
  append(bindings, stage.images, eStorageImage, flags);
  append(bindings, stage.buffers, eStorageBuffer, flags);
  Vector<vk::DescriptorSetLayoutBinding> uniforms;
  append(uniforms, stage.uniforms.size(), eUniformBufferDynamic, flags);
  for (auto *found : {&bindings, &uniforms}) {
    vk::DescriptorSetLayoutCreateInfo info{
      .bindingCount = static_cast<uint32_t>(found->size()),
      .pBindings = found->data()};
    layouts.emplace_back(GFX::STATE::gpu.logical, info);
  }
}

static auto reserve(const GFX::Stage &vertex, const GFX::Stage &fragment)
  -> vk::raii::DescriptorPool {
  Vector<vk::DescriptorPoolSize> sizes;
  auto add = [&sizes](vk::DescriptorType type, Whole count) {
    if (count) sizes.push_back({type, static_cast<uint32_t>(count)});
  };
  add(eCombinedImageSampler, vertex.samplers + fragment.samplers);
  add(eStorageImage, vertex.images + fragment.images);
  add(eStorageBuffer, vertex.buffers + fragment.buffers);
  add(eUniformBufferDynamic, vertex.uniforms.size() + fragment.uniforms.size());
  vk::DescriptorPoolCreateInfo info{
    .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
    .maxSets = SPACES,
    .poolSizeCount = static_cast<uint32_t>(sizes.size()),
    .pPoolSizes = sizes.data()};
  return vk::raii::DescriptorPool(GFX::STATE::gpu.logical, info);
}

void GFX::BACKEND::VULKAN::Descriptors::initialize(
  const GFX::Pipeline &description) {
  space(layouts, description.vertex, vk::ShaderStageFlagBits::eVertex);
  space(layouts, description.fragment, vk::ShaderStageFlagBits::eFragment);
  pool = reserve(description.vertex, description.fragment);
  Vector<vk::DescriptorSetLayout> spaces;
  for (auto &found : layouts) spaces.push_back(*found);
  vk::DescriptorSetAllocateInfo info{
    .descriptorPool = *pool,
    .descriptorSetCount = static_cast<uint32_t>(spaces.size()),
    .pSetLayouts = spaces.data()};
  for (auto &set : vk::raii::DescriptorSets(GFX::STATE::gpu.logical, info)) {
    sets.push_back(std::move(set));
  }
  for (auto &set : sets) handles.push_back(*set);
  for (auto size : description.vertex.uniforms) sizes.push_back(size);
  for (auto size : description.fragment.uniforms) sizes.push_back(size);
}

void GFX::BACKEND::VULKAN::Descriptors::close() {
  sizes.clear();
  handles.clear();
  sets.clear();
  pool.clear();
  layouts.clear();
}

#endif
