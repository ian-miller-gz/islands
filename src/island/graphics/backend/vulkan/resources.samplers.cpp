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

#if SR_GFX_BACKEND == SR_VULKAN

static Vector<GFX::Sampler> samplers;

static auto sample(GFX::SAMPLERS::Kind kind) -> GFX::Sampler {
  const auto edges = kind == GFX::SAMPLERS::REPEAT
                       ? vk::SamplerAddressMode::eRepeat
                       : vk::SamplerAddressMode::eClampToEdge;
  const auto filter =
    kind == GFX::SAMPLERS::LINEAR ? vk::Filter::eLinear : vk::Filter::eNearest;
  vk::SamplerCreateInfo info{
    .magFilter = filter,
    .minFilter = filter,
    .mipmapMode = vk::SamplerMipmapMode::eNearest,
    .addressModeU = edges,
    .addressModeV = edges,
    .addressModeW = edges};
  return GFX::Sampler(GFX::STATE::gpu.logical, info);
}

auto GFX::SAMPLERS::create(Kind kind) -> GFX::Handle {
  samplers.push_back(sample(kind));
  return samplers.size() - 1;
}

void GFX::SAMPLERS::destroy(GFX::Handle sampler) {
  GFX::BACKEND::VULKAN::RETIRED::file(std::move(samplers[sampler]));
  samplers[sampler] = GFX::Sampler(nullptr);
}

void GFX::SAMPLERS::close() { samplers.clear(); }

auto GFX::BACKEND::VULKAN::GET::sampler(GFX::Handle handle) -> GFX::Sampler& {
  return samplers[handle];
}

#endif
