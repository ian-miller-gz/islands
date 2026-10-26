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
#include <island/graphics/backend/vulkan/resources.internal.hpp>
#include <island/graphics/backend/vulkan/types.hpp>

namespace GFX::BACKEND::VULKAN {

constexpr Whole FRAMES = 2;

constexpr Integer UNHELD = -1;

struct Instance {
  vk::raii::Instance handle{nullptr};
  vk::raii::DebugUtilsMessengerEXT debug{nullptr};
  vk::raii::SurfaceKHR surface{nullptr};
  void initialize();
  void close();
};

struct Swapchain {
  vk::raii::SwapchainKHR handle{nullptr};
  Vector<vk::Image> images;
  Vector<vk::raii::ImageView> views;
  Vector<vk::raii::Framebuffer> targets;
  Vector<vk::raii::Semaphore> semaphores;
  GFX::Texture depth;
  vk::SurfaceFormatKHR format;
  vk::Extent2D extent;
  vk::PresentModeKHR mode = vk::PresentModeKHR::eFifo;
  Flag vsync = false;
  Integer presented = UNHELD;

  static auto select(const Vector<vk::SurfaceFormatKHR> &formats)
    -> vk::SurfaceFormatKHR;
  static auto select(const Vector<vk::PresentModeKHR> &modes)
    -> vk::PresentModeKHR;
  static auto name(vk::PresentModeKHR mode) -> STRING::Hot;
  static auto measure(const vk::SurfaceCapabilitiesKHR &capabilities)
    -> vk::Extent2D;
  static auto describe(
    const vk::SurfaceCapabilitiesKHR &capabilities,
    const vk::SurfaceFormatKHR &format, const vk::Extent2D &extent,
    vk::PresentModeKHR mode) -> vk::SwapchainCreateInfoKHR;

  void initialize();
  void attach();
  auto resize() -> Flag;
  void recreate(STRING::Hot reason);
  void close();
};

struct Frame {
  vk::raii::CommandBuffer commands{nullptr};
  vk::raii::Semaphore ready{nullptr};
  vk::raii::Fence busy{nullptr};
  Integer image = UNHELD;
  Retired retired;
};

struct Frames {
  vk::raii::CommandPool pool{nullptr};
  Vector<Frame> ring;
  Whole index = 0;
  void initialize();
  void close();
};

extern Instance instance;
extern Swapchain swapchain;
extern Frames frames;
extern vk::raii::RenderPass pass;

auto compose() -> vk::raii::RenderPass;
namespace COMMANDS {
auto create() -> vk::raii::CommandBuffers;
}  // namespace COMMANDS
void flush(const vk::raii::CommandBuffer &commands);

}  // namespace GFX::BACKEND::VULKAN
