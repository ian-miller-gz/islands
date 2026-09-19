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

#include <generated/values.hpp>
#include <generated/gfx.hpp>
#include <common/libraries.hpp>
#include <common/platform/selection.hpp>

#if !defined(SR_GFX_BACKEND)
#error \
  "SR_GFX_BACKEND is not defined; set it in configs/make.yaml (tokens: SR_GFX_BACKEND)."
#elif SR_GFX_BACKEND != SR_VULKAN && SR_GFX_BACKEND != SR_SDL3 && \
  SR_GFX_BACKEND != SR_WEBGPU
#error \
  "SR_GFX_BACKEND has an unknown value; expected SR_VULKAN, SR_SDL3, or SR_WEBGPU."
#endif

#if SR_PLATFORM == SR_WEB && SR_GFX_BACKEND != SR_WEBGPU
#error \
  "SR_PLATFORM=SR_WEB (the browser target) requires SR_GFX_BACKEND=SR_WEBGPU."
#elif SR_GFX_BACKEND == SR_WEBGPU && SR_PLATFORM != SR_WEB
#error \
  "SR_GFX_BACKEND=SR_WEBGPU is the browser backend; it requires SR_PLATFORM=SR_WEB."
#endif
