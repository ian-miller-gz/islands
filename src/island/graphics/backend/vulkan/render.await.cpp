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
#include <island/graphics/backend/vulkan/resources.internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#include <metrics.hpp>
#include <stdexcept>

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

static void wait(Frame &frame) {
  METRICS::Scope span("gfx.fence");
  if (
    GFX::STATE::gpu.logical.waitForFences(*frame.busy, true, UINT64_MAX) !=
    vk::Result::eSuccess) {
    throw std::runtime_error("failed waiting for the frame fence");
  }
}

static auto acquire(Frame &frame) -> Integer {
  METRICS::Scope span("gfx.await");
  try {
    auto [result, image] =
      swapchain.handle.acquireNextImage(UINT64_MAX, *frame.ready, nullptr);
    if (
      result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR) {
      return static_cast<Integer>(image);
    }
  } catch (const vk::OutOfDateKHRError &) {
  }
  swapchain.recreate("out of date on acquire");
  return UNHELD;
}

void GFX::BACKEND::await() {
  auto &frame = frames.ring[frames.index];
  if (frame.image != UNHELD) return;
  wait(frame);
  RETIRED::free();
  frame.image = acquire(frame);
}

void GFX::BACKEND::VULKAN::drop() {
  swapchain.presented = UNHELD;
  for (auto &frame : frames.ring) {
    frame.image = UNHELD;
    frame.ready =
      vk::raii::Semaphore(GFX::STATE::gpu.logical, vk::SemaphoreCreateInfo{});
  }
}

#endif
