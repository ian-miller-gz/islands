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

namespace GUI::SAC::LADDER {

constexpr STRING::Hot NAME = "ladder";

struct Rung {
  Whole span = 0;
  STRING::Hot name = "";
  String (*stated)(Whole at) = nullptr;
};

struct Mark {
  Integer at = 0;
  Flag opens = false;
  String label;
};

auto climb(const Vector<Rung> &rungs, Float scale, Float crowd) -> Rung;

auto climb(Whole grain, Whole factor, Float scale, Float crowd, Float seen)
  -> Whole;

auto dress(const Rung &rung, Whole above, Float pan, Float seen)
  -> Vector<Mark>;

#if SR_SAC == SR_NONE
inline auto climb(const Vector<Rung> &, Float, Float) -> Rung { return {}; }
inline auto climb(Whole, Whole, Float, Float, Float) -> Whole { return 0; }
inline auto dress(const Rung &, Whole, Float, Float) -> Vector<Mark> {
  return {};
}
#endif

}  // namespace GUI::SAC::LADDER
