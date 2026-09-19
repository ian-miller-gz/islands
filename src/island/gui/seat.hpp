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
#include <island/gui/gui.hpp>
#include <common.hpp>

namespace GUI::SAC::SEAT {

constexpr STRING::Hot NAME = "seat";

constexpr Whole NONE = static_cast<Whole>(-1);

auto named(STRING::Hot board, Whole row) -> String;

auto row(const String &id, STRING::Hot board) -> Whole;

auto selected(Handle document, STRING::Hot board) -> Whole;

#if SR_SAC == SR_NONE
inline auto named(STRING::Hot, Whole) -> String { return {}; }
inline auto row(const String &, STRING::Hot) -> Whole { return NONE; }
inline auto selected(Handle, STRING::Hot) -> Whole { return NONE; }
#endif

}  // namespace GUI::SAC::SEAT
