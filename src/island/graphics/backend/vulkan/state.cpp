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
#include <island/graphics/backend/vulkan/state.hpp>
#include <island/graphics/backend/vulkan/devices/internal.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

namespace GFX::BACKEND::DEVICES {
Gpu gpus[4] = {};
}  // namespace GFX::BACKEND::DEVICES

namespace GFX::BACKEND::STATE {
Float scale = 1.0f;
GFX::DEVICES::Gpu &gpu = GFX::DEVICES::gpus[0];
}  // namespace GFX::BACKEND::STATE

namespace GFX::BACKEND::VULKAN {
Instance instance = {};
Swapchain swapchain = {};
Frames frames = {};
vk::raii::RenderPass pass{nullptr};
}  // namespace GFX::BACKEND::VULKAN

#endif
