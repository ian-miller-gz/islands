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
#pragma once

#include <island/graphics/backend/passes.hpp>
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/backend/vulkan/types.hpp>

namespace GFX::BACKEND::VULKAN {

constexpr Whole SPACES = 4;

struct Descriptors {
  Vector<vk::raii::DescriptorSetLayout> layouts;
  vk::raii::DescriptorPool pool{nullptr};
  Vector<vk::raii::DescriptorSet> sets;
  Vector<vk::DescriptorSet> handles;
  Vector<Whole> sizes;
  void initialize(const GFX::Pipeline &description);
  void write(
    const GFX::Pipeline &description,
    const Vector<GFX::Buffer<Byte>> &uniforms);
  void close();
};

struct Pipeline {
  GFX::Pipeline description;
  Descriptors descriptors;
  Vector<GFX::Buffer<Byte>> uniforms;
  Whole capacity = 1;
  Whole cursor = 0;
  Whole counted = 0;
  Vector<uint32_t> offsets;
  GFX::Draw bound;
  vk::raii::PipelineLayout layout{nullptr};
  vk::raii::Pipeline handle{nullptr};

  void close() {
    handle.clear();
    layout.clear();
    descriptors.close();
    for (auto &buffer : uniforms) buffer.close();
    uniforms.clear();
    capacity = 1;
    cursor = 0;
    counted = 0;
    offsets.clear();
    bound = {};
  }
};

auto load(const GFX::Stage &stage) -> GFX::Shader;
auto build(
  const GFX::Shader &vertex, const GFX::Shader &fragment,
  const vk::raii::PipelineLayout &layout,
  const GFX::Pipeline &description) -> vk::raii::Pipeline;
namespace UNIFORM {
auto create(Whole size) -> GFX::Buffer<Byte>;
}  // namespace UNIFORM
void prepare();
auto feed(Pipeline &entry, const GFX::Draw &draw) -> const Vector<uint32_t> &;
void rebind(Pipeline &entry, const GFX::Draw &draw);
auto write(
  const vk::raii::DescriptorSet &set, Whole binding, vk::DescriptorType type,
  const vk::DescriptorBufferInfo &info) -> vk::WriteDescriptorSet;
auto write(
  const vk::raii::DescriptorSet &set, Whole binding, vk::DescriptorType type,
  const vk::DescriptorImageInfo &info) -> vk::WriteDescriptorSet;

}  // namespace GFX::BACKEND::VULKAN

namespace GFX::BACKEND::VULKAN::GET {
auto pipeline(GFX::Handle handle) -> Pipeline &;
}  // namespace GFX::BACKEND::VULKAN::GET
