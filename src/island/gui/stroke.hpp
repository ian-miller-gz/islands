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

namespace GUI::SAC::STROKE {

constexpr STRING::Hot NAME = "stroke";

struct Cell {
  Integer across = 0;
  Integer down = 0;
  auto operator==(const Cell &) const -> Bool = default;
};

struct Mark {
  Position at;
  Cell cell;
};

struct Mode {
  enum Over { GROUND, BOARD };
  STRING::Cold board;
  STRING::Cold name;
  Extent cell = {1.0f, 1.0f};
  Over over = GROUND;
};

struct Gesture {
  Flag opened = false;
  Flag standing = false;
  Flag closed = false;
  Flag dragged = false;
  Flag control = false;
  Flag shift = false;
  Flag alt = false;
  Mark from;
  Mark to;
};

auto draw(Handle document, const Mode &mode) -> Status;

namespace GET {

auto mode(Handle document) -> STRING::Cold;

auto gesture(Handle document) -> Gesture;

auto laid(Handle document) -> Vector<Cell>;

}  // namespace GET

#if SR_SAC == SR_NONE
inline auto draw(Handle, const Mode &) -> Status { return 1; }
inline auto GET::mode(Handle) -> STRING::Cold { return {}; }
inline auto GET::gesture(Handle) -> Gesture { return {}; }
inline auto GET::laid(Handle) -> Vector<Cell> { return {}; }
#endif

}  // namespace GUI::SAC::STROKE
