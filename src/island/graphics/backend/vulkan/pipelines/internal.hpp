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
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/backend/vulkan/types.hpp>

namespace GFX::BACKEND::VULKAN {

struct Fixed {
  vk::VertexInputBindingDescription binding{};
  Vector<vk::VertexInputAttributeDescription> attributes;
  vk::PipelineVertexInputStateCreateInfo input{};
  vk::PipelineInputAssemblyStateCreateInfo assembly{};
  vk::PipelineViewportStateCreateInfo viewport{
    .viewportCount = 1, .scissorCount = 1};
  vk::PipelineRasterizationStateCreateInfo raster{
    .polygonMode = vk::PolygonMode::eFill,
    .cullMode = vk::CullModeFlagBits::eNone,
    .frontFace = vk::FrontFace::eClockwise,
    .lineWidth = 1.0f};
  vk::PipelineMultisampleStateCreateInfo sampling{
    .rasterizationSamples = vk::SampleCountFlagBits::e1};
  vk::PipelineColorBlendAttachmentState attachment{
    .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
    .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
    .colorBlendOp = vk::BlendOp::eAdd,
    .srcAlphaBlendFactor = vk::BlendFactor::eSrcAlpha,
    .dstAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
    .alphaBlendOp = vk::BlendOp::eAdd,
    .colorWriteMask =
      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
  vk::PipelineColorBlendStateCreateInfo blending{.attachmentCount = 1};
  vk::PipelineDepthStencilStateCreateInfo depth{
    .depthCompareOp = vk::CompareOp::eLess};
  vk::DynamicState states[2] = {
    vk::DynamicState::eViewport, vk::DynamicState::eScissor};
  vk::PipelineDynamicStateCreateInfo dynamic{.dynamicStateCount = 2};
};

void inputs(Fixed &fixed, const Vector<Whole> &attributes);

}  // namespace GFX::BACKEND::VULKAN
