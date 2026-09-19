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

#include <common.hpp>
#include <island/graphics/backend/selection.hpp>

#if SR_GFX_BACKEND == SR_VULKAN

#include <island/graphics/backend/vulkan/state.hpp>

#elif SR_GFX_BACKEND == SR_SDL3

#include <island/graphics/backend/sdl-gpu/state.hpp>

#elif SR_GFX_BACKEND == SR_WEBGPU

#include <island/graphics/backend/webgpu/state.hpp>

#else
#error "Unsupported SR_GFX_BACKEND; expected SR_VULKAN, SR_SDL3, or SR_WEBGPU."
#endif