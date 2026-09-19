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
#include <utility>

#if SR_GFX_BACKEND == SR_VULKAN

using namespace GFX::BACKEND::VULKAN;

void GFX::BACKEND::VULKAN::Retired::close() {
  for (auto &buffer : buffers) buffer.close();
  buffers.clear();
  for (auto &texture : textures) texture.close();
  textures.clear();
  samplers.clear();
}

namespace GFX::BACKEND::VULKAN {
namespace {
auto slot() -> Frame * {
  if (frames.ring.empty()) return nullptr;
  return &frames.ring[frames.index];
}

void drain() {
  auto &gpu = GFX::STATE::gpu;
  if (*gpu.logical) gpu.logical.waitIdle();
}
}  // namespace
}  // namespace GFX::BACKEND::VULKAN

void GFX::BACKEND::VULKAN::RETIRED::file(GFX::Buffer<Byte> buffer) {
  auto *frame = slot();
  if (!frame) {
    drain();
    buffer.close();
    return;
  }
  frame->retired.buffers.push_back(std::move(buffer));
}

void GFX::BACKEND::VULKAN::RETIRED::file(GFX::Texture texture) {
  auto *frame = slot();
  if (!frame) {
    drain();
    texture.close();
    return;
  }
  frame->retired.textures.push_back(std::move(texture));
}

void GFX::BACKEND::VULKAN::RETIRED::file(GFX::Sampler sampler) {
  auto *frame = slot();
  if (!frame) {
    drain();
    sampler.clear();
    return;
  }
  frame->retired.samplers.push_back(std::move(sampler));
}

void GFX::BACKEND::VULKAN::RETIRED::free() {
  auto *frame = slot();
  if (frame) frame->retired.close();
}

#endif
