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

namespace GUI::SAC::CARRY {

constexpr STRING::Hot NAME = "carry";

struct Watch {
  STRING::Cold board;
  Walls walls;
  Float held = 0.0f;
  Float seated = 0.0f;
};

struct Drop {
  STRING::Cold id;
  Position at;
};

auto watch(Handle document, const Watch &watched) -> Status;

auto follow(Handle document, const Vector<String> &ids) -> Status;

namespace GET {

auto carried(Handle document) -> STRING::Cold;

auto dragged(Handle document, STRING::Hot id) -> Flag;

auto dropped(Handle document) -> Vector<Drop>;

}  // namespace GET

#if SR_SAC == SR_NONE
inline auto watch(Handle, const Watch &) -> Status { return 1; }
inline auto follow(Handle, const Vector<String> &ids) -> Status {
  return static_cast<Status>(ids.size());
}
inline auto GET::carried(Handle) -> STRING::Cold { return {}; }
inline auto GET::dragged(Handle, STRING::Hot) -> Flag { return false; }
inline auto GET::dropped(Handle) -> Vector<Drop> { return {}; }
#endif

}  // namespace GUI::SAC::CARRY
