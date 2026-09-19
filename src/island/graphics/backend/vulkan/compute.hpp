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

struct Kernel {
  GFX::Kernel description;
  Vector<vk::raii::DescriptorSetLayout> layouts;
  vk::raii::DescriptorPool pool{nullptr};
  Vector<vk::raii::DescriptorSet> sets;
  Vector<GFX::Buffer<Byte>> uniforms;
  GFX::Bindings bound;
  Flag primed = false;
  vk::raii::PipelineLayout layout{nullptr};
  vk::raii::Pipeline handle{nullptr};

  void close() {
    handle.clear();
    layout.clear();
    sets.clear();
    pool.clear();
    layouts.clear();
    for (auto &buffer : uniforms) buffer.close();
    uniforms.clear();
    bound = {};
    primed = false;
  }
};

void reserve(Kernel &kernel);
void point(Kernel &kernel);

void compute(
  const vk::raii::CommandBuffer &commands, const GFX::Dispatch &dispatch);
void boundary(const vk::raii::CommandBuffer &commands);

}  // namespace GFX::BACKEND::VULKAN

namespace GFX::BACKEND::VULKAN::GET {
auto kernel(GFX::Handle handle) -> Kernel &;
}  // namespace GFX::BACKEND::VULKAN::GET
