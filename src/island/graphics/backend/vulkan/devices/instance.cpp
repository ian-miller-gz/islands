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
#include <island/window/backend.hpp>
#include <iostream>

#if SR_GFX_BACKEND == SR_VULKAN

static VKAPI_ATTR vk::Bool32 VKAPI_CALL report(
  vk::DebugUtilsMessageSeverityFlagBitsEXT,
  vk::DebugUtilsMessageTypeFlagsEXT type,
  const vk::DebugUtilsMessengerCallbackDataEXT *data, void *) {
  std::cerr << "validation layer: type " << to_string(type)
            << " msg: " << data->pMessage << std::endl;
  return vk::False;
}

static auto context() -> vk::raii::Context & {
  static vk::raii::Context single;
  return single;
}

static auto layers() -> Vector<STRING::Hot> {
  Vector<STRING::Hot> found;
#ifndef NDEBUG
  found.push_back("VK_LAYER_KHRONOS_validation");
#endif
  return found;
}

static auto gather() -> Vector<STRING::Hot> {
  Vector<STRING::Hot> found = WINDOW::GET::extensions();
#ifndef NDEBUG
  found.push_back(vk::EXTDebugUtilsExtensionName);
#endif
  return found;
}

static auto messenger(vk::raii::Instance &instance)
  -> vk::raii::DebugUtilsMessengerEXT {
#ifdef NDEBUG
  return vk::raii::DebugUtilsMessengerEXT(nullptr);
#endif
  vk::DebugUtilsMessengerCreateInfoEXT info{
    .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                       vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                       vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
    .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                   vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                   vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
    .pfnUserCallback = &report};
  return instance.createDebugUtilsMessengerEXT(info);
}

void GFX::BACKEND::VULKAN::Instance::initialize() {
  vk::ApplicationInfo application{
    .pApplicationName = "Islands Engine",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = "Islands Engine",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = vk::ApiVersion14};
  auto required = layers();
  auto found = gather();
  vk::InstanceCreateInfo info{
    .pApplicationInfo = &application,
    .enabledLayerCount = static_cast<uint32_t>(required.size()),
    .ppEnabledLayerNames = required.data(),
    .enabledExtensionCount = static_cast<uint32_t>(found.size()),
    .ppEnabledExtensionNames = found.data()};
  handle = vk::raii::Instance(context(), info);
  debug = messenger(handle);
  surface = vk::raii::SurfaceKHR(handle, WINDOW::SURFACE::create(*handle));
}

void GFX::BACKEND::VULKAN::Instance::close() {
  surface.clear();
  debug.clear();
  handle.clear();
}

#endif
