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
#include <island/graphics/backend/vulkan/pipelines/internal.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

static auto format(Whole width) -> vk::Format {
  if (width == 1) return vk::Format::eR32Sfloat;
  if (width == 2) return vk::Format::eR32G32Sfloat;
  if (width == 3) return vk::Format::eR32G32B32Sfloat;
  return vk::Format::eR32G32B32A32Sfloat;
}

void GFX::BACKEND::VULKAN::inputs(
  Fixed &fixed, const Vector<Whole> &attributes) {
  if (attributes.empty()) return;
  Whole offset = 0;
  for (Whole slot = 0; slot < attributes.size(); slot += 1) {
    fixed.attributes.push_back(
      {.location = static_cast<uint32_t>(slot),
       .binding = 0,
       .format = format(attributes[slot]),
       .offset = static_cast<uint32_t>(offset)});
    offset += sizeof(Float) * attributes[slot];
  }
  fixed.binding = {
    .binding = 0,
    .stride = static_cast<uint32_t>(offset),
    .inputRate = vk::VertexInputRate::eVertex};
  fixed.input = {
    .vertexBindingDescriptionCount = 1,
    .pVertexBindingDescriptions = &fixed.binding,
    .vertexAttributeDescriptionCount =
      static_cast<uint32_t>(fixed.attributes.size()),
    .pVertexAttributeDescriptions = fixed.attributes.data()};
}

#endif
