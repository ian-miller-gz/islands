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
#include <island/gui/events.internal.hpp>
#include <island/gui/backend.hpp>

namespace STATE = GUI::STATE;

namespace {

constexpr Float MIDDLE = 0.5f;

auto stirred(const INPUT::Pointer &hand) -> Flag {
  return hand.dx != 0.0f || hand.dy != 0.0f || hand.wheel != 0.0f ||
         hand.left || hand.middle || hand.right || hand.rose.on || hand.fell.on;
}

}  // namespace

auto GUI::hover(Handle document, STRING::Hot id) -> Status {
  return hover(document, id, 0.0f, 0.0f);
}

auto GUI::hover(Handle document, STRING::Hot id, Float x, Float y) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  const auto extent = BACKEND::extent(document);
  if (extent.w <= 0.0f || extent.h <= 0.0f) return 1;
  auto &pointer = STATE::documents[document].pointer;
  if (pointer.holding) return 1;
  const auto corner = BACKEND::origin(document, node);
  const auto size = BACKEND::measured(document, node);
  const Float across = corner.x + size.w * ::MIDDLE + x;
  const Float down = corner.y + size.h * ::MIDDLE + y;
  const Flag inside =
    across >= 0.0f && across < extent.w && down >= 0.0f && down < extent.h;
  pointer.fed = true;
  pointer.resting = true;
  point(document, across, down, inside, false);
  return 0;
}

auto GUI::rest(Handle document, const INPUT::Pointer &hand) -> Flag {
  auto &pointer = STATE::documents[document].pointer;
  if (!pointer.resting) return false;
  if (::stirred(hand)) {
    pointer.resting = false;
    return false;
  }
  point(document, pointer.spot.x, pointer.spot.y, pointer.inside, false);
  return true;
}
