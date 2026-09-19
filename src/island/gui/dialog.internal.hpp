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
#include <island/gui/dialog.hpp>
#include <island/gui/gui.hpp>

namespace GUI::DIALOG {

constexpr STRING::Hot LISTING = "listing";
constexpr STRING::Hot WHERE = "where";
constexpr STRING::Hot FIELD = "field";
constexpr STRING::Hot OK = "ok";
constexpr STRING::Hot CANCEL = "cancel";

auto compose(Mode mode, const String &text) -> Handle;

void raise(Mode mode, STRING::Hot path, STRING::Hot filter);

auto raised() -> Handle;

struct Entry {
  String name;
  Flag directory = false;
};

auto walk(const String &directory, const String &filter) -> Vector<Entry>;
auto join(const String &directory, const String &name) -> String;
auto up(const String &directory) -> String;

#if SR_SAC == SR_NONE
inline void raise(Mode, STRING::Hot, STRING::Hot) {}
inline auto raised() -> Handle { return NONE; }
inline auto walk(const String &, const String &) -> Vector<Entry> { return {}; }
inline auto join(const String &, const String &name) -> String { return name; }
inline auto up(const String &directory) -> String { return directory; }
#endif

}  // namespace GUI::DIALOG
