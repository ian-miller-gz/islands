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
#include <stdexcept>

#if SR_GFX_BACKEND == SR_VULKAN

static auto suitable(const vk::raii::PhysicalDevice &device) -> Bool {
  return device.getProperties().deviceType ==
           vk::PhysicalDeviceType::eDiscreteGpu &&
         device.getFeatures().geometryShader;
}

static auto index(const vk::raii::PhysicalDevice &device) -> Whole {
  auto &surface = GFX::BACKEND::VULKAN::instance.surface;
  auto families = device.getQueueFamilyProperties();
  for (Whole i = 0; i < families.size(); i += 1) {
    if (
      (families[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
      device.getSurfaceSupportKHR(i, *surface)) {
      return i;
    }
  }
  throw std::runtime_error("no graphics + present queue family");
}

static auto device(const vk::raii::PhysicalDevice &physical, Whole family)
  -> vk::raii::Device {
  Float priority = 1.0f;
  vk::DeviceQueueCreateInfo queue{
    .queueFamilyIndex = static_cast<uint32_t>(family),
    .queueCount = 1,
    .pQueuePriorities = &priority};
  vk::PhysicalDeviceFeatures features{};
  Vector<STRING::Hot> extensions = {vk::KHRSwapchainExtensionName};
  vk::DeviceCreateInfo info{
    .queueCreateInfoCount = 1,
    .pQueueCreateInfos = &queue,
    .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
    .ppEnabledExtensionNames = extensions.data(),
    .pEnabledFeatures = &features};
  return vk::raii::Device(physical, info);
}

void GFX::BACKEND::DEVICES::Gpu::initialize() {
  auto &instance = GFX::BACKEND::VULKAN::instance;
  for (const auto &found : instance.handle.enumeratePhysicalDevices()) {
    if (suitable(found)) {
      physical = found;
      break;
    }
  }
  if (!*physical) throw std::runtime_error("no suitable GPU device");
  family = index(physical);
  logical = device(physical, family);
  queue = logical.getQueue(family, 0);
}

void GFX::BACKEND::DEVICES::Gpu::close() {
  queue.clear();
  logical.clear();
  physical.clear();
}

#endif
