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
#include <island/overlay/backend.hpp>
#include <cartridge.hpp>
#include <console.hpp>

namespace OVERLAY::WINDOWS::CARTRIDGE {
inline void prerender();
}  // namespace OVERLAY::WINDOWS::CARTRIDGE

namespace OVERLAY::WINDOWS::CONFIRM {
inline Flag active = false;
inline void prerender(STRING::Hot title, Flag &window, Flag &toggle);
}  // namespace OVERLAY::WINDOWS::CONFIRM

namespace OVERLAY::WINDOWS::DEBUG {
inline Flag active = true;
inline void prerender();
};  // namespace OVERLAY::WINDOWS::DEBUG

namespace OVERLAY::WINDOWS::DEMO {
inline Flag active = false;
inline void prerender();
};  // namespace OVERLAY::WINDOWS::DEMO

inline void OVERLAY::WINDOWS::CARTRIDGE::prerender() {
  OVERLAY::BACKEND::WINDOWS::CARTRIDGE::prerender();
}

inline void OVERLAY::WINDOWS::CONFIRM::prerender(
  STRING::Hot title, Flag &window, Flag &toggle) {
  if (!active) return;
  OVERLAY::BACKEND::WINDOWS::CONFIRM::prerender(title, window, toggle);
}

inline void OVERLAY::WINDOWS::DEBUG::prerender() {
  if (!active) return;
  OVERLAY::BACKEND::WINDOWS::DEBUG::prerender();
}

inline void OVERLAY::WINDOWS::DEMO::prerender() {
  if (!active) return;
  OVERLAY::BACKEND::WINDOWS::DEMO::prerender();
}

namespace OVERLAY::WINDOWS {
inline void preset(const String &name, Flag active) {
  if (name == "debug") return void(DEBUG::active = active);
  if (name == "console") return void(::CONSOLES::visible = active);
  if (name == "demo") return void(DEMO::active = active);
  ::CARTRIDGE::preset(name, active);
}
}  // namespace OVERLAY::WINDOWS