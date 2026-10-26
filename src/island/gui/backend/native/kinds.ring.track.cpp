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
#include <island/gui/backend/native/kinds.internal.hpp>
#include <island/gui/backend/native/kinds.ring.hpp>
#include <cmath>

auto GUI::KINDS::faint(const Properties &properties) -> GFX::Color {
  const auto ground = washed(properties, properties.color);
  const Flag stated = properties.track.a > 0.0f;
  const auto &ink = stated ? properties.track : properties.edge;
  const Float top = stated ? ink.a : ink.a * TRACK;
  const Float alpha = top + ground.a * (1.0f - top);
  if (alpha <= 0.0f) return GFX::COLORS::CLEAR;
  const Float share = top / alpha;
  return {
    std::lerp(ground.r, ink.r, share), std::lerp(ground.g, ink.g, share),
    std::lerp(ground.b, ink.b, share), alpha};
}
