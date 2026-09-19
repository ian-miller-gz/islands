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

namespace GUI::SAC::ROWS {

constexpr STRING::Hot NAME = "rows";

constexpr Whole NONE = static_cast<Whole>(-1);

struct Row {
  Float depth = 0.0f;
  Flag parted = false;
};

struct Rail {
  Float top = 0.0f;
  Float depth = 0.0f;
};

auto stack(const Vector<Row> &rows, Float margin, Float gap) -> Vector<Rail>;

auto deep(const Vector<Rail> &rails, Float margin) -> Float;

auto at(const Vector<Rail> &rails, Float down) -> Whole;

auto reach(Float pan, Float seen) -> Float;

auto reach(Float reached, Float pan, Float seen, Float spare) -> Float;

#if SR_SAC == SR_NONE
inline auto stack(const Vector<Row> &, Float, Float) -> Vector<Rail> {
  return {};
}
inline auto deep(const Vector<Rail> &, Float margin) -> Float { return margin; }
inline auto at(const Vector<Rail> &, Float) -> Whole { return NONE; }
inline auto reach(Float pan, Float) -> Float { return pan; }
inline auto reach(Float reached, Float, Float, Float) -> Float {
  return reached;
}
#endif

}  // namespace GUI::SAC::ROWS
