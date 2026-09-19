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
#include <cstring>
#define LOGGER_CATEGORY "~/island/graphics/vulkan::pipelines"

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

static Vector<Pipeline> pipelines;

static auto assemble(const Descriptors &descriptors)
  -> vk::raii::PipelineLayout {
  Vector<vk::DescriptorSetLayout> handles;
  for (auto &found : descriptors.layouts) handles.push_back(*found);
  vk::PipelineLayoutCreateInfo info{
    .setLayoutCount = static_cast<uint32_t>(handles.size()),
    .pSetLayouts = handles.data()};
  return vk::raii::PipelineLayout(GFX::STATE::gpu.logical, info);
}

auto GFX::PIPELINES::create(const GFX::Pipeline &description) -> GFX::Handle {
  auto &entry = pipelines.emplace_back();
  entry.description = description;
  auto vertex = load(description.vertex);
  auto fragment = load(description.fragment);
  entry.descriptors.initialize(description);
  entry.layout = assemble(entry.descriptors);
  if (*vertex && *fragment)
    entry.handle = build(vertex, fragment, entry.layout, description);
  for (auto size : description.vertex.uniforms)
    entry.uniforms.push_back(UNIFORM::create(size));
  for (auto size : description.fragment.uniforms)
    entry.uniforms.push_back(UNIFORM::create(size));
  entry.descriptors.write(description, entry.uniforms);
  LOGGER::get(LOGGER_CATEGORY)
    .debug(
      "Pipeline created: %s + %s", description.vertex.path.c_str(),
      description.fragment.path.c_str());
  return pipelines.size() - 1;
}

void GFX::PIPELINES::write(GFX::Handle pipeline, Whole slot, const void *data) {
  auto &entry = pipelines[pipeline];
  auto &vertex = entry.description.vertex.uniforms;
  auto &fragment = entry.description.fragment.uniforms;
  auto size =
    slot < vertex.size() ? vertex[slot] : fragment[slot - vertex.size()];
  std::memcpy(entry.uniforms[slot].data, data, size);
}

void GFX::PIPELINES::destroy(GFX::Handle pipeline) {
  GFX::STATE::gpu.logical.waitIdle();
  pipelines[pipeline].close();
}

void GFX::PIPELINES::close() {
  for (auto &entry : pipelines) entry.close();
  pipelines.clear();
}

auto GFX::BACKEND::VULKAN::GET::pipeline(GFX::Handle handle) -> Pipeline & {
  return pipelines[handle];
}

#endif
