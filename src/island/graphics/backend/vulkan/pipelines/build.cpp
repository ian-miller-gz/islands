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
#include <island/graphics/backend/vulkan/devices/internal.hpp>
#include <island/graphics/backend/vulkan/pipelines.hpp>
#include <island/graphics/backend/vulkan/pipelines/internal.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

using GFX::BACKEND::VULKAN::Fixed;

static auto stage(const GFX::Shader &shader, vk::ShaderStageFlagBits flags)
  -> vk::PipelineShaderStageCreateInfo {
  return {.stage = flags, .module = *shader, .pName = "main"};
}

static void wire(Fixed &fixed, const GFX::Pipeline &description) {
  fixed.blending.pAttachments = &fixed.attachment;
  fixed.dynamic.pDynamicStates = fixed.states;
  fixed.assembly.topology = description.topology == GFX::LINES
                              ? vk::PrimitiveTopology::eLineList
                              : vk::PrimitiveTopology::eTriangleList;
  fixed.attachment.blendEnable = description.blend;
  fixed.depth.depthTestEnable = description.depth;
  fixed.depth.depthWriteEnable = description.depth;
  GFX::BACKEND::VULKAN::inputs(fixed, description.attributes);
}

auto GFX::BACKEND::VULKAN::build(
  const GFX::Shader &vertex, const GFX::Shader &fragment,
  const vk::raii::PipelineLayout &layout,
  const GFX::Pipeline &description) -> vk::raii::Pipeline {
  vk::PipelineShaderStageCreateInfo stages[] = {
    stage(vertex, vk::ShaderStageFlagBits::eVertex),
    stage(fragment, vk::ShaderStageFlagBits::eFragment)};
  Fixed fixed;
  wire(fixed, description);
  vk::GraphicsPipelineCreateInfo info{
    .stageCount = 2,
    .pStages = stages,
    .pVertexInputState = &fixed.input,
    .pInputAssemblyState = &fixed.assembly,
    .pViewportState = &fixed.viewport,
    .pRasterizationState = &fixed.raster,
    .pMultisampleState = &fixed.sampling,
    .pDepthStencilState = &fixed.depth,
    .pColorBlendState = &fixed.blending,
    .pDynamicState = &fixed.dynamic,
    .layout = *layout,
    .renderPass = *pass};
  return vk::raii::Pipeline(GFX::STATE::gpu.logical, nullptr, info);
}

#endif
