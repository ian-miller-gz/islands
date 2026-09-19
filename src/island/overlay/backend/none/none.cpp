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
#include <common.hpp>
#define LOGGER_CATEGORY "~/island/overlay/backend::"

#if SR_OVERLAY_BACKEND == SR_NONE

void *OVERLAY::BACKEND::context = nullptr;

void OVERLAY::BACKEND::initialize() {
  auto &logger = LOGGER::get(LOGGER_CATEGORY "initialize");
  logger.info("Overlay backend: none (ImGui compiled out).");
}

void OVERLAY::BACKEND::prerender() {}

void OVERLAY::BACKEND::PRERENDER::complete() {}

void OVERLAY::BACKEND::close() {}

auto OVERLAY::GET::keyboard() -> Flag { return false; }

auto OVERLAY::GET::pointer() -> Flag { return false; }

void OVERLAY::BACKEND::WINDOWS::CARTRIDGE::prerender() {}

void OVERLAY::BACKEND::WINDOWS::CONFIRM::prerender(
  STRING::Hot, Flag &, Flag &) {}

void OVERLAY::BACKEND::WINDOWS::DEBUG::prerender() {}

void OVERLAY::BACKEND::WINDOWS::DEMO::prerender() {}

#endif
