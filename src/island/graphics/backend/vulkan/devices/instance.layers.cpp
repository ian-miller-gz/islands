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
#include <island.hpp>
#include <island/graphics/backend/vulkan/devices/internal.hpp>
#include <algorithm>
#include <string_view>

#if SR_GFX_BACKEND == SR_VULKAN

auto GFX::BACKEND::VULKAN::INSTANCE::context() -> vk::raii::Context & {
  static vk::raii::Context single;
  return single;
}

auto GFX::BACKEND::VULKAN::INSTANCE::listed(STRING::Hot layer) -> Flag {
  return std::ranges::any_of(
    context().enumerateInstanceLayerProperties(),
    [layer](const vk::LayerProperties &properties) {
      return std::string_view(properties.layerName.data()) == layer;
    });
}

auto GFX::BACKEND::VULKAN::INSTANCE::offered(STRING::Hot extension) -> Flag {
  return std::ranges::any_of(
    context().enumerateInstanceExtensionProperties(),
    [extension](const vk::ExtensionProperties &properties) {
      return std::string_view(properties.extensionName.data()) == extension;
    });
}

#endif
