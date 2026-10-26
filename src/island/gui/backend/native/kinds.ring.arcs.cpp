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
#include <island/gui/backend/native/kinds.ring.hpp>
#include <island/gui/backend/native/kinds.internal.hpp>
#include <algorithm>
#include <cmath>

namespace {

using GUI::KINDS::Arc;

auto fill(const GUI::Properties &properties) -> Arc {
  if (properties.most <= properties.least) return {.filled = true};
  const Flag centred = properties.least < 0.0f && properties.most > 0.0f;
  const Float origin = centred ? GUI::KINDS::share(properties, 0.0f) : 0.0f;
  const Float value = GUI::KINDS::share(properties, properties.value);
  return {std::min(origin, value), std::max(origin, value), 0, true};
}

auto deal(const Arc &arc) -> Whole {
  const Float length = arc.to - arc.from;
  if (length <= 0.0f) return 0;
  const Float count = static_cast<Float>(GUI::KINDS::PIECES);
  const auto pieces = static_cast<Whole>(std::lround(length * count));
  return std::max(GUI::KINDS::LEAST, pieces);
}

}  // namespace

auto GUI::KINDS::share(const Properties &properties, Float value) -> Float {
  const Float span = properties.most - properties.least;
  return std::clamp((value - properties.least) / span, 0.0f, 1.0f);
}

auto GUI::KINDS::arcs(const Properties &properties) -> std::array<Arc, ARCS> {
  const auto filled = ::fill(properties);
  std::array<Arc, ARCS> arcs = {{
    {0.0f, filled.from, 0, false},
    filled,
    {filled.to, 1.0f, 0, false},
  }};
  Whole dealt = 0;
  for (auto &arc : arcs) {
    arc.pieces = ::deal(arc);
    dealt += arc.pieces;
  }
  auto &largest = *std::ranges::max_element(arcs, {}, &Arc::pieces);
  largest.pieces = largest.pieces + PIECES - dealt;
  return arcs;
}
