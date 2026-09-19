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

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <common.hpp>

namespace GFX {

template <typename Type>
struct Buffer {
  vk::raii::Buffer handle{nullptr};
  vk::raii::DeviceMemory memory{nullptr};
  Type *data = nullptr;

  void close() {
    if (data) memory.unmapMemory();
    data = nullptr;
    handle.clear();
    memory.clear();
  }
};

struct Texture {
  vk::raii::Image image{nullptr};
  vk::raii::DeviceMemory memory{nullptr};
  vk::raii::ImageView view{nullptr};

  void close() {
    view.clear();
    image.clear();
    memory.clear();
  }
};

using Shader = vk::raii::ShaderModule;

using Sampler = vk::raii::Sampler;

}  // namespace GFX

namespace GFX::BACKEND::VULKAN::DEPTH {
constexpr auto FORMAT = vk::Format::eD32Sfloat;
}  // namespace GFX::BACKEND::VULKAN::DEPTH

namespace GFX::BACKEND::DEVICES {
struct Gpu {
  vk::raii::PhysicalDevice physical{nullptr};
  vk::raii::Device logical{nullptr};
  vk::raii::Queue queue{nullptr};
  Whole family = 0;

  void initialize();
  void close();
};
extern Gpu gpus[4];
}  // namespace GFX::BACKEND::DEVICES

namespace GFX {
namespace DEVICES = GFX::BACKEND::DEVICES;
}
