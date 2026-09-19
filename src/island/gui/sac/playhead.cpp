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
#include <algorithm>
#include <cmath>

#include <island/gui/playhead.hpp>
#include <island/gui/sac.hpp>
#include <logger.hpp>

static const String category = "~/island/gui::";

namespace {
constexpr Float LEAST = 2.0f;
constexpr Float HALF = 2.0f;
constexpr Float STILL = 0.0f;

Flag told = false;

auto refused() -> Flag {
  if (GUI::SAC::GET::adopted(GUI::SAC::PLAYHEAD::NAME)) return false;
  if (!::told)
    LOGGER::get(category + "playhead")
      .error(
        "The playhead is not designated: add `playhead: true` under the "
        "manifest's `gui:` block (or adopt it through GUI::SAC::adopt).");
  ::told = true;
  return true;
}

auto stroke(Float hair) -> Float { return std::max(::LEAST, std::round(hair)); }
}  // namespace

auto GUI::SAC::PLAYHEAD::blade(Float at, Float scale, Float hair) -> Mark {
  if (::refused()) return {at, 0.0f};
  const Float face = ::stroke(hair);
  return {at, scale > ::STILL ? face / scale : face};
}

auto GUI::SAC::PLAYHEAD::stripcap(Float at, Float pan, Float scale, Float wide)
  -> Mark {
  if (::refused()) return {};
  return {(at - pan) * scale - wide / ::HALF, wide};
}
