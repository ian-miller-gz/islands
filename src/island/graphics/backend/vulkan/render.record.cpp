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
#include <island/graphics/backend/passes.hpp>
#include <island/graphics/backend/vulkan/compute.hpp>
#include <island/graphics/backend/vulkan/devices/internal.hpp>
#include <island/graphics/backend/vulkan/pipelines.hpp>
#include <island/graphics/backend/vulkan/render.internal.hpp>
#include <island/graphics/backend/vulkan/resources.internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#include <metrics.hpp>
#include <algorithm>

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

struct Placement {
  vk::RenderPass pass;
  vk::Framebuffer target;
  vk::Extent2D extent;
};

static auto place(const GFX::Pass &pass, Whole image) -> Placement {
  if (pass.target == GFX::NONE) {
    return {
      *GFX::BACKEND::VULKAN::pass, *swapchain.targets[image], swapchain.extent};
  }
  auto &target = GET::target(pass.target);
  return {GET::offscreen(), *target.framebuffer, target.extent};
}

static void arm(
  const vk::raii::CommandBuffer &commands, const GFX::Pass &pass,
  vk::Extent2D extent) {
  if (pass.draws.empty()) return;
  auto &viewport = pass.viewport;
  commands.setViewport(
    0, vk::Viewport{
         viewport.x, viewport.y + viewport.h, viewport.w, -viewport.h, 0.0f,
         1.0f});
  commands.setScissor(0, vk::Rect2D{{0, 0}, extent});
}

static auto rect(const GFX::Viewport &scissor, vk::Extent2D extent)
  -> vk::Rect2D {
  const auto width = static_cast<int32_t>(extent.width);
  const auto height = static_cast<int32_t>(extent.height);
  const auto left = std::clamp(static_cast<int32_t>(scissor.x), 0, width);
  const auto top = std::clamp(static_cast<int32_t>(scissor.y), 0, height);
  const auto right =
    std::clamp(static_cast<int32_t>(scissor.x + scissor.w), left, width);
  const auto bottom =
    std::clamp(static_cast<int32_t>(scissor.y + scissor.h), top, height);
  return {
    {left, top},
    {static_cast<uint32_t>(right - left), static_cast<uint32_t>(bottom - top)}};
}

static auto clip(
  const vk::raii::CommandBuffer &commands, const GFX::Draw &draw,
  vk::Extent2D extent, Flag clipped) -> Flag {
  if (!GFX::clips(draw.scissor)) {
    if (clipped) commands.setScissor(0, vk::Rect2D{{0, 0}, extent});
    return false;
  }
  commands.setScissor(0, ::rect(draw.scissor, extent));
  return true;
}

static void execute(
  const vk::raii::CommandBuffer &commands, const GFX::Pass &pass, Whole image,
  Flag last) {
  auto where = place(pass, image);
  vk::ClearValue values[] = {
    vk::ClearValue{
      .color = vk::ClearColorValue{std::array<Float, 4>{
        pass.clear.r, pass.clear.g, pass.clear.b, pass.clear.a}}},
    vk::ClearValue{.depthStencil = {GFX::PASSES::FAR, 0}}};
  vk::RenderPassBeginInfo info{
    .renderPass = where.pass,
    .framebuffer = where.target,
    .renderArea = {.offset = {0, 0}, .extent = where.extent},
    .clearValueCount = 2,
    .pClearValues = values};
  commands.beginRenderPass(info, vk::SubpassContents::eInline);
  arm(commands, pass, where.extent);
  Flag clipped = false;
  for (auto &draw : pass.draws) {
    clipped = clip(commands, draw, where.extent, clipped);
    GFX::BACKEND::VULKAN::draw(commands, draw);
  }
  METRICS::count("gfx.draws", static_cast<Whole>(pass.draws.size()));
  if (last) overlay(commands);
  commands.endRenderPass();
}

void GFX::BACKEND::VULKAN::record(Whole image) {
  prepare();
  auto &commands = frames.ring[frames.index].commands;
  commands.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
  unbind();
  PATCHES::record(commands);
  auto &dispatches = GFX::DISPATCHES::list();
  for (auto &dispatch : dispatches) VULKAN::compute(commands, dispatch);
  if (!dispatches.empty()) VULKAN::boundary(commands);
  auto &passes = GFX::PASSES::list();
  static const GFX::Pass EMPTY = {};
  Flag hosted = passes.empty() || passes.back().target != GFX::NONE;
  for (Whole i = 0; i < passes.size(); i += 1) {
    execute(commands, passes[i], image, !hosted && i + 1 == passes.size());
  }
  if (hosted) execute(commands, EMPTY, image, true);
  commands.end();
}

#endif
