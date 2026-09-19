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

#include <island/gui/sac.selection.hpp>
#include <common.hpp>
#include <island/graphics/backend/passes.hpp>

namespace GUI::DIALOG {

constexpr STRING::Hot NAME = "dialog";

enum Mode { OPEN, SAVE };

enum Result { INACTIVE, PENDING, ACCEPTED, CANCELLED };

void open(Mode mode, STRING::Hot path, STRING::Hot filter = "");

auto place(GFX::Viewport viewport) -> Status;

auto poll() -> Result;

void flush(GFX::Pass &pass);

void close();

namespace GET {

auto active() -> Flag;

auto path() -> String;

auto mode() -> Mode;

}  // namespace GET

#if SR_SAC == SR_NONE
inline void open(Mode, STRING::Hot, STRING::Hot) {}
inline auto place(GFX::Viewport) -> Status { return 1; }
inline auto poll() -> Result { return INACTIVE; }
inline void flush(GFX::Pass &) {}
inline void close() {}
inline auto GET::active() -> Flag { return false; }
inline auto GET::path() -> String { return String(); }
inline auto GET::mode() -> Mode { return OPEN; }
#endif

}  // namespace GUI::DIALOG
