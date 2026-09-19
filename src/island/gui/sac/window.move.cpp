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
#include <island/gui/window.internal.hpp>

namespace {
constexpr Float STILL = 0.0f;

constexpr Float HALVED = 2.0f;
}  // namespace

auto GUI::SAC::WINDOW::about(Float pan, Float at, Float scale, Float wanted)
  -> Float {
  if (refused()) return pan;
  if (scale <= ::STILL || wanted <= ::STILL) return pan;
  return at - (at - pan) * scale / wanted;
}

auto GUI::SAC::WINDOW::chase(Float pan, Float at, Float seen) -> Float {
  if (refused()) return pan;
  if (seen <= ::STILL) return pan;
  const Float middle = pan + seen / ::HALVED;
  return at < middle ? pan : at - seen / ::HALVED;
}

auto GUI::SAC::WINDOW::centre(Float pan, Float at, Float size, Float seen)
  -> Float {
  if (refused()) return pan;
  if (seen <= ::STILL) return pan;
  if (at >= pan && at + size <= pan + seen) return pan;
  return at + (size - seen) / ::HALVED;
}
