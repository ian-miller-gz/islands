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

namespace GUI::TIP {

constexpr STRING::Hot NAME = "tip";

constexpr Whole REST = 30;

constexpr STRING::Hot PLATE = "tip";

constexpr STRING::Hot LABEL = "tip.label";

constexpr STRING::Hot HINT = ".hint";

void note(Handle document, STRING::Hot id, const String &line);

namespace GET {

auto note(Handle document, STRING::Hot id) -> String;

auto notes(Handle document, const String &prefix) -> Vector<String>;

}  // namespace GET

#if SR_SAC == SR_NONE
inline void note(Handle, STRING::Hot, const String &) {}
inline auto GET::note(Handle, STRING::Hot) -> String { return {}; }
inline auto GET::notes(Handle, const String &) -> Vector<String> { return {}; }
#endif

}  // namespace GUI::TIP
