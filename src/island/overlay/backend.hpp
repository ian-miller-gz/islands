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
#include <island/overlay/backend/selection.hpp>

#if SR_OVERLAY_BACKEND == SR_IMGUI
#include <island/overlay/backend/imgui/imgui.hpp>
#elif SR_OVERLAY_BACKEND == SR_NONE
#include <island/overlay/backend/none/none.hpp>
#else
#error "Unsupported SR_OVERLAY_BACKEND; expected SR_IMGUI or SR_NONE."
#endif

namespace OVERLAY::BACKEND {
void initialize();
void prerender();
void close();
}  // namespace OVERLAY::BACKEND

namespace OVERLAY::BACKEND::PRERENDER {
void complete();
}  // namespace OVERLAY::BACKEND::PRERENDER

namespace OVERLAY::BACKEND::WINDOWS::CARTRIDGE {
void prerender();
}  // namespace OVERLAY::BACKEND::WINDOWS::CARTRIDGE

namespace OVERLAY::BACKEND::WINDOWS::CONFIRM {
void prerender(STRING::Hot title, Flag &active, Flag &toggle);
}  // namespace OVERLAY::BACKEND::WINDOWS::CONFIRM

namespace OVERLAY::BACKEND::WINDOWS::DEBUG {
void prerender();
}  // namespace OVERLAY::BACKEND::WINDOWS::DEBUG

namespace OVERLAY::BACKEND::WINDOWS::DEMO {
void prerender();
}  // namespace OVERLAY::BACKEND::WINDOWS::DEMO
