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
#include <island/graphics/backend/vulkan/resources.internal.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#include <utility>
#define LOGGER_CATEGORY "~/island/graphics/vulkan::targets"

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

static Vector<Target> targets;
static vk::raii::RenderPass offscreen{nullptr};

auto GFX::TARGETS::create(Whole width, Whole height) -> GFX::Handle {
  auto handle = adopt(TARGET::create(width, height), width, height);
  auto depth = DEPTH::create(width, height);
  vk::ImageView attachments[] = {*GET::texture(handle).view, *depth.view};
  vk::Extent2D extent{
    static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  vk::FramebufferCreateInfo info{
    .renderPass = GET::offscreen(),
    .attachmentCount = 2,
    .pAttachments = attachments,
    .width = extent.width,
    .height = extent.height,
    .layers = 1};
  if (targets.size() <= handle) targets.resize(handle + 1);
  targets[handle] = {
    std::move(depth), vk::raii::Framebuffer(GFX::STATE::gpu.logical, info),
    extent};
  LOGGER::get(LOGGER_CATEGORY)
    .debug(
      "Target created: %zu x %zu", static_cast<size_t>(width),
      static_cast<size_t>(height));
  return handle;
}

void GFX::TARGETS::destroy(GFX::Handle target) {
  targets[target].framebuffer.clear();
  targets[target].depth.close();
  GFX::TEXTURES::destroy(target);
}

void GFX::TARGETS::close() {
  for (Whole handle = 0; handle < targets.size(); handle += 1) {
    if (*targets[handle].framebuffer) destroy(handle);
  }
  targets.clear();
  offscreen.clear();
}

auto GFX::BACKEND::VULKAN::GET::target(GFX::Handle handle) -> Target& {
  return targets[handle];
}

auto GFX::BACKEND::VULKAN::GET::offscreen() -> vk::RenderPass {
  if (!*::offscreen) ::offscreen = TARGET::compose();
  return *::offscreen;
}

#endif
