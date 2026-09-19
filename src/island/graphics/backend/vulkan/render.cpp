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
#include <island/graphics/backend/render.hpp>
#include <island/graphics/backend/vulkan/devices/internal.hpp>
#include <island/graphics/backend/vulkan/render.internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#include <metrics.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

static void submit(Frame &frame, Whole image) {
  vk::PipelineStageFlags stage(
    vk::PipelineStageFlagBits::eColorAttachmentOutput);
  vk::SubmitInfo info{
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &(*frame.ready),
    .pWaitDstStageMask = &stage,
    .commandBufferCount = 1,
    .pCommandBuffers = &(*frame.commands),
    .signalSemaphoreCount = 1,
    .pSignalSemaphores = &(*swapchain.semaphores[image])};
  GFX::STATE::gpu.queue.submit(info, *frame.busy);
}

static void present(Whole image) {
  METRICS::Scope span("gfx.present");
  auto index = static_cast<uint32_t>(image);
  vk::PresentInfoKHR info{
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &(*swapchain.semaphores[image]),
    .swapchainCount = 1,
    .pSwapchains = &(*swapchain.handle),
    .pImageIndices = &index};
  try {
    const auto result = GFX::STATE::gpu.queue.presentKHR(info);
    swapchain.presented = static_cast<Integer>(image);
    if (result != vk::Result::eSuboptimalKHR) return;
  } catch (const vk::OutOfDateKHRError &) {
  }
  swapchain.recreate("out of date on present");
}

void GFX::BACKEND::render() {
  if (!VULKAN::swapchain.resize()) return;
  GFX::BACKEND::await();
  auto &frame = VULKAN::frames.ring[VULKAN::frames.index];
  if (frame.image == VULKAN::UNHELD) return;
  const auto image = static_cast<Whole>(frame.image);
  GFX::STATE::gpu.logical.resetFences(*frame.busy);
  frame.commands.reset();
  {
    METRICS::Scope span("gfx.record");
    VULKAN::record(image);
  }
  submit(frame, image);
  VULKAN::capture(image);
  VULKAN::viewports();
  present(image);
  frame.image = VULKAN::UNHELD;
  VULKAN::frames.index = (VULKAN::frames.index + 1) % VULKAN::FRAMES;
}

#endif
