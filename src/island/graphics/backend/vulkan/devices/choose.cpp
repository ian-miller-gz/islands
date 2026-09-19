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
#include <algorithm>
#include <island.hpp>
#include <island/graphics/backend/vulkan/devices/internal.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

using Swapchain = GFX::BACKEND::VULKAN::Swapchain;

auto Swapchain::select(const Vector<vk::SurfaceFormatKHR> &formats)
  -> vk::SurfaceFormatKHR {
  for (const auto &format : formats) {
    if (
      format.format == vk::Format::eB8G8R8A8Unorm &&
      format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
      return format;
    }
  }
  return formats[0];
}

auto Swapchain::select(const Vector<vk::PresentModeKHR> &modes)
  -> vk::PresentModeKHR {
  if (GFX::VSYNC::on) return vk::PresentModeKHR::eFifo;
  for (const auto &mode : modes) {
    if (mode == vk::PresentModeKHR::eMailbox) return mode;
  }
  for (const auto &mode : modes) {
    if (mode == vk::PresentModeKHR::eImmediate) return mode;
  }
  return vk::PresentModeKHR::eFifo;
}

auto Swapchain::name(const vk::PresentModeKHR mode) -> STRING::Hot {
  switch (mode) {
    case vk::PresentModeKHR::eFifo:
      return "fifo";
    case vk::PresentModeKHR::eFifoRelaxed:
      return "relaxed";
    case vk::PresentModeKHR::eMailbox:
      return "mailbox";
    case vk::PresentModeKHR::eImmediate:
      return "immediate";
    default:
      return "unknown";
  }
}

auto Swapchain::measure(const vk::SurfaceCapabilitiesKHR &capabilities)
  -> vk::Extent2D {
  if (
    capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }
  return {
    std::clamp<uint32_t>(
      GFX::WINDOWS::MAIN::width, capabilities.minImageExtent.width,
      capabilities.maxImageExtent.width),
    std::clamp<uint32_t>(
      GFX::WINDOWS::MAIN::height, capabilities.minImageExtent.height,
      capabilities.maxImageExtent.height)};
}

namespace GFX::BACKEND::VULKAN {
namespace {
constexpr Whole QUEUED = 2;
constexpr Whole SPARED = 3;

auto count(
  const vk::SurfaceCapabilitiesKHR &capabilities,
  const vk::PresentModeKHR mode) -> Whole {
  Whole minimum = std::max<Whole>(
    mode == vk::PresentModeKHR::eFifo ? QUEUED : SPARED,
    capabilities.minImageCount);
  if (0 < capabilities.maxImageCount && capabilities.maxImageCount < minimum) {
    minimum = capabilities.maxImageCount;
  }
  return minimum;
}
}  // namespace
}  // namespace GFX::BACKEND::VULKAN

auto Swapchain::describe(
  const vk::SurfaceCapabilitiesKHR &capabilities,
  const vk::SurfaceFormatKHR &format, const vk::Extent2D &extent,
  const vk::PresentModeKHR mode) -> vk::SwapchainCreateInfoKHR {
  return {
    .surface = *GFX::BACKEND::VULKAN::instance.surface,
    .minImageCount =
      static_cast<uint32_t>(GFX::BACKEND::VULKAN::count(capabilities, mode)),
    .imageFormat = format.format,
    .imageColorSpace = format.colorSpace,
    .imageExtent = extent,
    .imageArrayLayers = 1,
    .imageUsage = vk::ImageUsageFlagBits::eColorAttachment |
                  vk::ImageUsageFlagBits::eTransferSrc,
    .imageSharingMode = vk::SharingMode::eExclusive,
    .preTransform = capabilities.currentTransform,
    .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
    .presentMode = mode,
    .clipped = true,
    .oldSwapchain = nullptr};
}

#endif
