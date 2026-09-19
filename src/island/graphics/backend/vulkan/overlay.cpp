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
#include <island/graphics/backend/vulkan/render.internal.hpp>
#include <island/window/backend.hpp>

#if SR_GFX_BACKEND == SR_VULKAN
#if SR_OVERLAY_BACKEND == SR_IMGUI
#include <imgui/backends/imgui_impl_vulkan.h>

using namespace GFX::BACKEND::VULKAN;

static PFN_vkVoidFunction resolve(STRING::Hot name, void *) {
  return instance.handle.getDispatcher()->vkGetInstanceProcAddr(
    *instance.handle, name);
}

void GFX::BACKEND::OVERLAY::initialize() {
  WINDOW::attach();
  ImGui_ImplVulkan_LoadFunctions(vk::ApiVersion14, &resolve);
  auto &gpu = GFX::STATE::gpu;
  auto count = static_cast<uint32_t>(swapchain.images.size());
  ImGui_ImplVulkan_InitInfo info = {};
  info.ApiVersion = vk::ApiVersion14;
  info.Instance = *instance.handle;
  info.PhysicalDevice = *gpu.physical;
  info.Device = *gpu.logical;
  info.QueueFamily = static_cast<uint32_t>(gpu.family);
  info.Queue = *gpu.queue;
  info.DescriptorPoolSize = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE;
  info.MinImageCount = count;
  info.ImageCount = count;
  info.PipelineInfoMain.RenderPass = *pass;
  info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  ImGui_ImplVulkan_Init(&info);
}

void GFX::BACKEND::OVERLAY::frame() {
  ImGui_ImplVulkan_NewFrame();
  WINDOW::frame();
}

void GFX::BACKEND::OVERLAY::close() {
  GFX::STATE::gpu.logical.waitIdle();
  ImGui_ImplVulkan_Shutdown();
  WINDOW::detach();
}

void GFX::BACKEND::VULKAN::overlay(const vk::raii::CommandBuffer &commands) {
  if (!::OVERLAY::context) return;
  ImDrawData *data = ImGui::GetDrawData();
  if (data) ImGui_ImplVulkan_RenderDrawData(data, *commands);
}

void GFX::BACKEND::VULKAN::viewports() {
  if (!::OVERLAY::context) return;
  if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)) return;
  ImGui::UpdatePlatformWindows();
  ImGui::RenderPlatformWindowsDefault();
}

#else

void GFX::BACKEND::OVERLAY::initialize() {}
void GFX::BACKEND::OVERLAY::frame() {}
void GFX::BACKEND::OVERLAY::close() {}
void GFX::BACKEND::VULKAN::overlay(const vk::raii::CommandBuffer &) {}
void GFX::BACKEND::VULKAN::viewports() {}

#endif
#endif
