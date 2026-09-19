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
#include <island/graphics/backend/resources.hpp>
#include <island/graphics/backend/vulkan/types.hpp>

namespace GFX::BACKEND::VULKAN {

namespace MAPPED {
auto create(Whole size, GFX::BUFFERS::Kind kind) -> GFX::Buffer<Byte>;
}  // namespace MAPPED
namespace READBACK {
auto create(Whole size) -> GFX::Buffer<Byte>;
}  // namespace READBACK
namespace STAGING {
auto create(Whole size) -> GFX::Buffer<Byte>;
}  // namespace STAGING

namespace PATCHES {
void stage(
  GFX::Handle texture, const void *pixels, Whole x, Whole y, Whole width,
  Whole height, Whole depth, Flag fresh);
void record(const vk::raii::CommandBuffer &commands);
void close();
}  // namespace PATCHES
struct Retired {
  Vector<GFX::Buffer<Byte>> buffers;
  Vector<GFX::Texture> textures;
  Vector<GFX::Sampler> samplers;
  void close();
};

namespace RETIRED {
void file(GFX::Buffer<Byte> buffer);
void file(GFX::Texture texture);
void file(GFX::Sampler sampler);
void free();
}  // namespace RETIRED

namespace IMAGE {
auto create(
  Whole width, Whole height,
  vk::Format format = vk::Format::eR8G8B8A8Unorm) -> GFX::Texture;
}  // namespace IMAGE
namespace DEPTH {
auto create(Whole width, Whole height) -> GFX::Texture;
}  // namespace DEPTH
namespace TARGET {
auto create(Whole width, Whole height) -> GFX::Texture;
auto compose() -> vk::raii::RenderPass;
}  // namespace TARGET
namespace FIELD {
auto create(Whole width, Whole height) -> GFX::Texture;
}  // namespace FIELD

struct Target {
  GFX::Texture depth;
  vk::raii::Framebuffer framebuffer{nullptr};
  vk::Extent2D extent;
};

auto adopt(
  GFX::Texture texture, Whole width, Whole height,
  Flag storage = false) -> GFX::Handle;

void back(GFX::Texture &texture);
void upload(
  GFX::Texture &texture, const void *pixels, Whole width, Whole height,
  Whole depth = GFX::TEXTURES::STRIDE);
void barrier(
  const vk::raii::CommandBuffer &commands, vk::Image image,
  vk::ImageLayout from, vk::ImageLayout to);

}  // namespace GFX::BACKEND::VULKAN

namespace GFX::BACKEND::VULKAN::GET {
auto buffer(GFX::Handle handle) -> GFX::Buffer<Byte> &;
auto texture(GFX::Handle handle) -> GFX::Texture &;
auto storage(GFX::Handle handle) -> Flag;
auto sampler(GFX::Handle handle) -> GFX::Sampler &;
auto target(GFX::Handle handle) -> Target &;
auto offscreen() -> vk::RenderPass;
auto memory(const vk::MemoryRequirements &needs, vk::MemoryPropertyFlags wanted)
  -> uint32_t;
}  // namespace GFX::BACKEND::VULKAN::GET
