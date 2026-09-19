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
#include <island/window/backend.hpp>

#if SR_WINDOW_BACKEND == SR_GLFW
#if SR_OVERLAY_BACKEND == SR_IMGUI
#include <island/window/backend/glfw/window.internal.hpp>
#include <imgui/backends/imgui_impl_glfw.h>

void WINDOW::BACKEND::attach() {
  ImGui_ImplGlfw_InitForVulkan(GLFW::window, true);
}

void WINDOW::BACKEND::frame() { ImGui_ImplGlfw_NewFrame(); }

void WINDOW::BACKEND::detach() { ImGui_ImplGlfw_Shutdown(); }

#endif
#endif
