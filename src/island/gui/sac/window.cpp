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

#include <island/gui/window.internal.hpp>
#include <island/gui/sac.hpp>
#include <logger.hpp>

static const String category = "~/island/gui::";

namespace {
constexpr Float NOTHING = 0.0f;

constexpr Float ORIGIN = 0.0f;

Flag told = false;
}  // namespace

auto GUI::SAC::WINDOW::refused() -> Flag {
  if (GUI::SAC::GET::adopted(GUI::SAC::WINDOW::NAME)) return false;
  if (!::told)
    LOGGER::get(category + "window")
      .error(
        "The window is not designated: add `window: true` under the "
        "manifest's `gui:` block (or adopt it through GUI::SAC::adopt).");
  ::told = true;
  return true;
}

auto GUI::SAC::WINDOW::seen(const Extent &measured, const NGA::Zoom &zoom)
  -> Extent {
  if (refused()) return {};
  const Float down = zoom.down == NGA::Zoom::SAME ? zoom.value : zoom.down;
  if (zoom.value <= ::NOTHING || down <= ::NOTHING) return {};
  return {
    measured.w <= ::NOTHING ? ::NOTHING : measured.w / zoom.value,
    measured.h <= ::NOTHING ? ::NOTHING : measured.h / down};
}

auto GUI::SAC::WINDOW::least(Float measured, Float span) -> Float {
  if (measured <= ::NOTHING || span <= ::NOTHING) return ::NOTHING;
  return measured / span;
}

auto GUI::SAC::WINDOW::walled(
  const Walls &box, const Extent &wander, const Extent &seen) -> Walls {
  const Float west = std::min(::ORIGIN, box.west) - wander.w;
  const Float north = std::min(::ORIGIN, box.north) - wander.h;
  return {
    west, std::max(std::max(::ORIGIN, box.east) + wander.w, west + seen.w),
    north, std::max(std::max(::ORIGIN, box.south) + wander.h, north + seen.h)};
}
