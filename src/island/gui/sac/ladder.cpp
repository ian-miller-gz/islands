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
#include <cmath>

#include <island/gui/ladder.hpp>
#include <island/gui/sac.hpp>
#include <logger.hpp>

static const String category = "~/island/gui::";

namespace {
constexpr Whole SPARE = 2;
constexpr Float NOWHERE = 0.0f;
constexpr Whole STEP = 2;

Flag told = false;

auto refused() -> Flag {
  if (GUI::SAC::GET::adopted(GUI::SAC::LADDER::NAME)) return false;
  if (!::told)
    LOGGER::get(category + "ladder")
      .error(
        "The ladder is not designated: add `ladder: true` under the "
        "manifest's `gui:` block (or adopt it through GUI::SAC::adopt).");
  ::told = true;
  return true;
}
}  // namespace

auto GUI::SAC::LADDER::climb(
  const Vector<Rung> &rungs, Float scale, Float crowd) -> Rung {
  for (const Rung &rung : rungs)
    if (Float(rung.span) * scale >= crowd) return rung;
  return rungs.empty() ? Rung{} : rungs.back();
}

auto GUI::SAC::LADDER::climb(
  Whole grain, Whole factor, Float scale, Float crowd, Float seen) -> Whole {
  if (grain == 0 || factor < ::STEP) return grain;
  Whole span = grain;
  while (Float(span) * scale < crowd && Float(span) < seen) span *= factor;
  return span;
}

auto GUI::SAC::LADDER::dress(
  const Rung &rung, Whole above, Float pan, Float seen) -> Vector<Mark> {
  Vector<Mark> marks;
  if (::refused()) return marks;
  const Float wide = Float(rung.span);
  if (wide <= ::NOWHERE || seen <= ::NOWHERE) return marks;
  const Integer opened = Integer(std::floor(pan / wide));
  const Whole stood = Whole(seen / wide) + ::SPARE;
  for (Whole tick = 0; tick < stood; ++tick) {
    Mark mark;
    mark.at = (opened + Integer(tick)) * Integer(rung.span);
    mark.opens = above > 0 && mark.at % Integer(above) == 0;
    if (rung.stated && mark.at >= 0) mark.label = rung.stated(Whole(mark.at));
    marks.push_back(mark);
  }
  return marks;
}
