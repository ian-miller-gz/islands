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

#include <island/gui/nga.hpp>
#include <island/gui/sac.selection.hpp>
#include <island/gui/gui.hpp>
#include <common.hpp>

namespace GUI::SAC::WINDOW {

constexpr STRING::Hot NAME = "window";

auto seen(const Extent &measured, const NGA::Zoom &zoom) -> Extent;

auto least(Float measured, Float span) -> Float;

auto about(Float pan, Float at, Float scale, Float wanted) -> Float;

auto chase(Float pan, Float at, Float seen) -> Float;

auto centre(Float pan, Float at, Float size, Float seen) -> Float;

auto walled(const Walls &box, const Extent &wander, const Extent &seen)
  -> Walls;

#if SR_SAC == SR_NONE
inline auto seen(const Extent &, const NGA::Zoom &) -> Extent { return {}; }
inline auto least(Float, Float) -> Float { return 0.0f; }
inline auto about(Float pan, Float, Float, Float) -> Float { return pan; }
inline auto chase(Float pan, Float, Float) -> Float { return pan; }
inline auto centre(Float pan, Float, Float, Float) -> Float { return pan; }
inline auto walled(const Walls &, const Extent &, const Extent &) -> Walls {
  return {};
}
#endif

}  // namespace GUI::SAC::WINDOW
