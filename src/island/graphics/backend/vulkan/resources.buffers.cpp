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

static Vector<GFX::Buffer<Byte>> buffers;

auto GFX::BUFFERS::create(Whole size, Kind kind) -> GFX::Handle {
  buffers.push_back(GFX::BACKEND::VULKAN::MAPPED::create(size, kind));
  return buffers.size() - 1;
}

auto GFX::BUFFERS::GET::data(GFX::Handle buffer) -> void* {
  return buffers[buffer].data;
}

void GFX::BUFFERS::destroy(GFX::Handle buffer) {
  GFX::BACKEND::VULKAN::RETIRED::file(std::move(buffers[buffer]));
  buffers[buffer] = {};
}

void GFX::BUFFERS::close() {
  for (auto& buffer : buffers) buffer.close();
  buffers.clear();
}

auto GFX::BACKEND::VULKAN::GET::buffer(GFX::Handle handle)
  -> GFX::Buffer<Byte>& {
  return buffers[handle];
}

#endif
