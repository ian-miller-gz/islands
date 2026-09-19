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
#include <generated/window.hpp>
#include <common/libraries.hpp>
#include <common/platform/selection.hpp>

#if !defined(SR_WINDOW_BACKEND)
#error \
  "SR_WINDOW_BACKEND is not defined; set it in configs/make.yaml (tokens: SR_WINDOW_BACKEND)."
#elif SR_WINDOW_BACKEND != SR_SDL3 && SR_WINDOW_BACKEND != SR_GLFW && \
  SR_WINDOW_BACKEND != SR_WEB
#error \
  "SR_WINDOW_BACKEND has an unknown value; expected SR_SDL3, SR_GLFW, or SR_WEB."
#endif

#if SR_GFX_BACKEND == SR_SDL3 && SR_WINDOW_BACKEND != SR_SDL3
#error "SR_GFX_BACKEND=SR_SDL3 (SDL-GPU) requires SR_WINDOW_BACKEND=SR_SDL3."
#endif

#if SR_PLATFORM == SR_WEB && SR_WINDOW_BACKEND != SR_WEB
#error \
  "SR_PLATFORM=SR_WEB (the browser target) requires SR_WINDOW_BACKEND=SR_WEB."
#elif SR_WINDOW_BACKEND == SR_WEB && SR_PLATFORM != SR_WEB
#error \
  "SR_WINDOW_BACKEND=SR_WEB is the browser canvas backend; it requires SR_PLATFORM=SR_WEB."
#endif
