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
#include <island/graphics/backend/vulkan/devices/internal.hpp>
#include <island/graphics/backend/vulkan/resources.internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

static auto attachment() -> vk::AttachmentDescription {
  return {
    .format = swapchain.format.format,
    .samples = vk::SampleCountFlagBits::e1,
    .loadOp = vk::AttachmentLoadOp::eClear,
    .storeOp = vk::AttachmentStoreOp::eStore,
    .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
    .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
    .initialLayout = vk::ImageLayout::eUndefined,
    .finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal};
}

static auto buffer() -> vk::AttachmentDescription {
  return {
    .format = DEPTH::FORMAT,
    .samples = vk::SampleCountFlagBits::e1,
    .loadOp = vk::AttachmentLoadOp::eClear,
    .storeOp = vk::AttachmentStoreOp::eDontCare,
    .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
    .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
    .initialLayout = vk::ImageLayout::eUndefined,
    .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal};
}

static auto before() -> vk::SubpassDependency {
  using enum vk::PipelineStageFlagBits;
  return {
    .srcSubpass = VK_SUBPASS_EXTERNAL,
    .dstSubpass = 0,
    .srcStageMask =
      eFragmentShader | eColorAttachmentOutput | eEarlyFragmentTests,
    .dstStageMask = eColorAttachmentOutput | eEarlyFragmentTests,
    .srcAccessMask = vk::AccessFlagBits::eNone,
    .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite |
                     vk::AccessFlagBits::eDepthStencilAttachmentWrite};
}

static auto after() -> vk::SubpassDependency {
  return {
    .srcSubpass = 0,
    .dstSubpass = VK_SUBPASS_EXTERNAL,
    .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
    .dstStageMask = vk::PipelineStageFlagBits::eFragmentShader,
    .srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
    .dstAccessMask = vk::AccessFlagBits::eShaderRead};
}

auto GFX::BACKEND::VULKAN::TARGET::compose() -> vk::raii::RenderPass {
  vk::AttachmentDescription attachments[] = {attachment(), buffer()};
  vk::AttachmentReference reference{
    .attachment = 0, .layout = vk::ImageLayout::eColorAttachmentOptimal};
  vk::AttachmentReference depth{
    .attachment = 1, .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal};
  vk::SubpassDescription subpass{
    .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
    .colorAttachmentCount = 1,
    .pColorAttachments = &reference,
    .pDepthStencilAttachment = &depth};
  vk::SubpassDependency edges[] = {before(), after()};
  vk::RenderPassCreateInfo info{
    .attachmentCount = 2,
    .pAttachments = attachments,
    .subpassCount = 1,
    .pSubpasses = &subpass,
    .dependencyCount = 2,
    .pDependencies = edges};
  return vk::raii::RenderPass(GFX::STATE::gpu.logical, info);
}

#endif
