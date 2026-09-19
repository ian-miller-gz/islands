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
#include <cstring>
#define LOGGER_CATEGORY "~/island/graphics/vulkan::compute"

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

static Vector<Kernel> kernels;

auto GFX::COMPUTE::create(const GFX::Kernel &description) -> GFX::Handle {
  auto &kernel = kernels.emplace_back();
  kernel.description = description;
  auto shader = load(description.stage);
  reserve(kernel);
  Vector<vk::DescriptorSetLayout> handles;
  for (auto &layout : kernel.layouts) handles.push_back(*layout);
  vk::PipelineLayoutCreateInfo assembly{
    .setLayoutCount = static_cast<uint32_t>(handles.size()),
    .pSetLayouts = handles.data()};
  kernel.layout = vk::raii::PipelineLayout(GFX::STATE::gpu.logical, assembly);
  if (*shader) {
    vk::PipelineShaderStageCreateInfo shading{
      .stage = vk::ShaderStageFlagBits::eCompute,
      .module = *shader,
      .pName = "main"};
    vk::ComputePipelineCreateInfo info{
      .stage = shading, .layout = *kernel.layout};
    kernel.handle = vk::raii::Pipeline(GFX::STATE::gpu.logical, nullptr, info);
  }
  for (auto size : description.stage.uniforms)
    kernel.uniforms.push_back(UNIFORM::create(size));
  point(kernel);
  LOGGER::get(LOGGER_CATEGORY)
    .debug("Kernel created: %s", description.stage.path.c_str());
  return kernels.size() - 1;
}

void GFX::COMPUTE::write(GFX::Handle kernel, Whole slot, const void *data) {
  auto &entry = kernels[kernel];
  std::memcpy(
    entry.uniforms[slot].data, data, entry.description.stage.uniforms[slot]);
}

void GFX::COMPUTE::destroy(GFX::Handle kernel) {
  GFX::STATE::gpu.logical.waitIdle();
  kernels[kernel].close();
}

void GFX::COMPUTE::close() {
  for (auto &kernel : kernels) kernel.close();
  kernels.clear();
}

auto GFX::BACKEND::VULKAN::GET::kernel(GFX::Handle handle) -> Kernel & {
  return kernels[handle];
}

#endif
