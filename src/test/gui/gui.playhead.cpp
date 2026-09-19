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
#include <island/gui/playhead.hpp>
#include <test/gui/gui.internal.hpp>

namespace {
constexpr Float SCALE = 2.0f;
constexpr Float AT = 10.0f;
constexpr Float PAN = 4.0f;
constexpr Float HAIR = 4.0f;
constexpr Float CAP = 6.0f;
constexpr Float STILL = 0.0f;
}  // namespace

Status bladed() {
  using namespace GUI::SAC;
  Status status = check(
    "undesignated draws nothing",
    PLAYHEAD::blade(::AT, ::SCALE, ::HAIR).wide == 0.0f);
  status |= check("the name adopts", adopt({String(PLAYHEAD::NAME)}) == 0);
  const PLAYHEAD::Mark blade = PLAYHEAD::blade(::AT, ::SCALE, ::HAIR);
  status |= check("the blade stands where the clock is", blade.at == ::AT);
  status |= check(
    "and its weight is the face stroke over the scale", blade.wide == 2.0f);
  status |= check(
    "a fractional face stroke rounds whole",
    PLAYHEAD::blade(::AT, 1.0f, 3.4f).wide == 3.0f);
  status |= check(
    "and never thins under the raster floor",
    PLAYHEAD::blade(::AT, 1.0f, 1.0f).wide == 2.0f);
  status |= check(
    "an unmeasured board answers the face number",
    PLAYHEAD::blade(::AT, ::STILL, ::HAIR).wide == ::HAIR);
  const PLAYHEAD::Mark cap = PLAYHEAD::stripcap(::AT, ::PAN, ::SCALE, ::CAP);
  status |=
    check("the cap applies the board's transform by hand", cap.at == 9.0f);
  status |= check("and stands the width the face states", cap.wide == ::CAP);
  status |= check(
    "a clock west of the pan lands off the face",
    PLAYHEAD::stripcap(2.0f, ::PAN, ::SCALE, ::CAP).at == -7.0f);
  status |= check("re-adoption replaces whole", adopt({}) == 0);
  status |= check(
    "the blade closed again",
    PLAYHEAD::blade(::AT, ::SCALE, ::HAIR).wide == 0.0f);
  status |= check(
    "and the cap with it",
    PLAYHEAD::stripcap(::AT, ::PAN, ::SCALE, ::CAP).wide == 0.0f);
  return status;
}
