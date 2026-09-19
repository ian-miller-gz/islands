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

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

auto GFX::BACKEND::VULKAN::write(
  const vk::raii::DescriptorSet &set, Whole binding, vk::DescriptorType type,
  const vk::DescriptorBufferInfo &info) -> vk::WriteDescriptorSet {
  return {
    .dstSet = *set,
    .dstBinding = static_cast<uint32_t>(binding),
    .descriptorCount = 1,
    .descriptorType = type,
    .pBufferInfo = &info};
}

auto GFX::BACKEND::VULKAN::write(
  const vk::raii::DescriptorSet &set, Whole binding, vk::DescriptorType type,
  const vk::DescriptorImageInfo &info) -> vk::WriteDescriptorSet {
  return {
    .dstSet = *set,
    .dstBinding = static_cast<uint32_t>(binding),
    .descriptorCount = 1,
    .descriptorType = type,
    .pImageInfo = &info};
}

void GFX::BACKEND::VULKAN::Descriptors::write(
  const GFX::Pipeline &description, const Vector<GFX::Buffer<Byte>> &uniforms) {
  constexpr auto UNIFORM = vk::DescriptorType::eUniformBufferDynamic;
  Vector<vk::DescriptorBufferInfo> infos;
  infos.reserve(uniforms.size());
  Vector<vk::WriteDescriptorSet> writes;
  auto &vertex = description.vertex.uniforms;
  Whole count = vertex.size();
  for (Whole slot = 0; slot < uniforms.size(); slot += 1) {
    auto size =
      slot < count ? vertex[slot] : description.fragment.uniforms[slot - count];
    infos.push_back({*uniforms[slot].handle, 0, size});
    auto &set = slot < count ? sets[1] : sets[3];
    writes.push_back(VULKAN::write(
      set, slot < count ? slot : slot - count, UNIFORM, infos.back()));
  }
  GFX::STATE::gpu.logical.updateDescriptorSets(writes, {});
}

#endif
