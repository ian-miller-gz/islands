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

#include <island/overlay/backend.hpp>
#include <island/overlay/widgets.hpp>
#include <island/overlay/windows.hpp>

namespace OVERLAY {
inline void initialize();
inline void prerender();
inline void close();
}  // namespace OVERLAY

namespace OVERLAY::GET {
auto keyboard() -> Flag;
auto pointer() -> Flag;
}  // namespace OVERLAY::GET

inline void OVERLAY::initialize() { OVERLAY::BACKEND::initialize(); }

inline void OVERLAY::close() { OVERLAY::BACKEND::close(); }

inline void OVERLAY::prerender() {
  if (!OVERLAY::BACKEND::context) return;
  OVERLAY::BACKEND::prerender();
  OVERLAY::WINDOWS::DEBUG::prerender();
  OVERLAY::WINDOWS::CARTRIDGE::prerender();
  OVERLAY::WINDOWS::DEMO::prerender();
  OVERLAY::BACKEND::PRERENDER::complete();
}