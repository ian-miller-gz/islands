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
#include <island/graphics/backend/spirv.hpp>
#include <island/graphics/backend/vulkan/state.hpp>
#include <island/graphics/backend/vulkan/pipelines.hpp>
#define LOGGER_CATEGORY "~/island/graphics/vulkan::shaders"

#if SR_GFX_BACKEND == SR_VULKAN

static auto read(const String &filename) -> Vector<Byte> {
  Vector<Byte> buffer;
  if (IO::read(filename, buffer) != 0 || buffer.empty())
    LOGGER::get(LOGGER_CATEGORY)
      .error("Cannot open shader: %s", filename.c_str());
  return buffer;
}

static auto code(const GFX::Stage &stage) -> Vector<Byte> {
  if (stage.bytes.empty()) return read(stage.path + ".spv");
  if (GFX::BACKEND::SPIRV::vet(stage.bytes)) return stage.bytes;
  LOGGER::get(LOGGER_CATEGORY)
    .error(
      "Runtime SPIR-V refused: %zu bytes are not a module", stage.bytes.size());
  return {};
}

auto GFX::BACKEND::VULKAN::load(const GFX::Stage &stage) -> GFX::Shader {
  LOGGER::get(LOGGER_CATEGORY)
    .debug(
      "Loading shader: %s",
      stage.bytes.empty() ? stage.path.c_str() : "<memory>");
  auto code = ::code(stage);
  if (code.empty()) return GFX::Shader{nullptr};
  vk::ShaderModuleCreateInfo info{
    .codeSize = code.size(),
    .pCode = reinterpret_cast<const uint32_t *>(code.data())};
  return GFX::Shader(GFX::STATE::gpu.logical, info);
}

#endif
