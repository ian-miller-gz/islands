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
#include <island/graphics/backend/vulkan/render.internal.hpp>
#include <island/graphics/backend/vulkan/resources.internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#include <island/graphics/capture.hpp>
#include <utility>

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

namespace {

struct Rect {
  Whole x = 0;
  Whole y = 0;
  Whole w = 0;
  Whole h = 0;
};

auto swapped() -> Flag {
  return swapchain.format.format == vk::Format::eB8G8R8A8Srgb ||
         swapchain.format.format == vk::Format::eB8G8R8A8Unorm;
}

auto held(const Rect &wanted) -> Rect {
  const auto extent = swapchain.extent;
  if (wanted.x >= extent.width || wanted.y >= extent.height) return {};
  const Whole across = extent.width - wanted.x;
  const Whole down = extent.height - wanted.y;
  return {
    wanted.x, wanted.y, wanted.w < across ? wanted.w : across,
    wanted.h < down ? wanted.h : down};
}

}  // namespace

static void copy(Whole image, const Rect &rect, const GFX::Buffer<Byte> &out) {
  auto recording = COMMANDS::create();
  auto &recorder = recording.front();
  barrier(
    recorder, swapchain.images[image], vk::ImageLayout::ePresentSrcKHR,
    vk::ImageLayout::eTransferSrcOptimal);
  vk::BufferImageCopy region{
    .imageSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1},
    .imageOffset =
      {static_cast<int32_t>(rect.x), static_cast<int32_t>(rect.y), 0},
    .imageExtent = {
      static_cast<uint32_t>(rect.w), static_cast<uint32_t>(rect.h), 1}};
  recorder.copyImageToBuffer(
    swapchain.images[image], vk::ImageLayout::eTransferSrcOptimal, *out.handle,
    region);
  barrier(
    recorder, swapchain.images[image], vk::ImageLayout::eTransferSrcOptimal,
    vk::ImageLayout::ePresentSrcKHR);
  flush(recorder);
}

void GFX::BACKEND::VULKAN::capture(Whole image) {
  if (!GFX::CAPTURE::armed) return;
  GFX::STATE::gpu.queue.waitIdle();
  auto extent = swapchain.extent;
  auto out =
    READBACK::create(extent.width * extent.height * GFX::CAPTURE::CHANNELS);
  copy(image, {0, 0, extent.width, extent.height}, out);
  GFX::CAPTURE::write(extent.width, extent.height, out.data, swapped());
  out.close();
}

auto GFX::CAPTURE::read(Whole x, Whole y, Whole w, Whole h) -> WINDOW::Image {
  if (swapchain.presented == UNHELD) return {};
  const Rect rect = held({x, y, w, h});
  if (!rect.w || !rect.h) return {};
  auto out = READBACK::create(rect.w * rect.h * CHANNELS);
  copy(static_cast<Whole>(swapchain.presented), rect, out);
  WINDOW::Image picture{
    rect.w, rect.h,
    Vector<Byte>(out.data, out.data + rect.w * rect.h * CHANNELS)};
  out.close();
  if (swapped())
    for (Whole pixel = 0; pixel < rect.w * rect.h; pixel += 1)
      std::swap(
        picture.pixels[pixel * CHANNELS], picture.pixels[pixel * CHANNELS + 2]);
  return picture;
}

#endif
