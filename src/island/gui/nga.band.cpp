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
#include <island/gui/nga.internal.hpp>
#include <island/gui/backend.hpp>
#include <algorithm>
#include <cmath>

auto GUI::NGA::pending(const Document &entry) -> Whole {
  const auto held = entry.pointer.wiring;
  if (held == PARENTLESS || !entry.nodes[held].live) return PARENTLESS;
  return board(entry, held);
}

auto GUI::NGA::banding(const Document &entry) -> Whole {
  const auto deck = entry.pointer.banding;
  if (deck == PARENTLESS || !entry.nodes[deck].live) return PARENTLESS;
  return deck;
}

auto GUI::NGA::band(const Document &entry) -> Span {
  const auto from = entry.pointer.grab;
  const auto to = entry.pointer.reach;
  return {
    std::min(from.x, to.x), std::min(from.y, to.y), std::abs(to.x - from.x),
    std::abs(to.y - from.y)};
}

auto GUI::NGA::caught(Handle document, Whole index, const Span &band) -> Flag {
  const auto corner = BACKEND::origin(document, index);
  const auto size = BACKEND::measured(document, index);
  return corner.x < band.x + band.w && band.x < corner.x + size.w &&
         corner.y < band.y + band.h && band.y < corner.y + size.h;
}
