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
#include <island/gui/backend/native/native.internal.hpp>
#include <island/gui/backend/native/kinds.snap.hpp>
#include <island/gui/nga.internal.hpp>
#include <island/graphics/backend/resources.hpp>

namespace NGA = GUI::NGA;

namespace {

auto seated(const NGA::Span &mark, const GFX::Color &ink) -> GUI::Element {
  namespace NATIVE = GUI::BACKEND::NATIVE;
  const Float side = NATIVE::snapped(mark.w, NATIVE::HAIRLINE);
  return {NATIVE::snapped(mark.x),
          NATIVE::snapped(mark.y),
          side,
          side,
          GUI::FULL,
          ink};
}

}  // namespace

void GUI::BACKEND::NATIVE::guide(
  const Document &entry, const Chrome &chrome, Whole board, Element *seat) {
  const auto aimed = entry.pointer.snapped;
  const Flag standing = aimed != PARENTLESS && aimed < chrome.slots.size() &&
                        chrome.slots[aimed].shown;
  STATE::writes += NGA::DASHES;
  if (!standing) {
    for (Whole at = 0; at < NGA::DASHES; at += 1) seat[at] = {};
    return;
  }
  NGA::Span marks[NGA::DASHES];
  NGA::guide(
    entry.pointer.reach, NATIVE::centre(chrome.slots[aimed]),
    NGA::THICKNESS * chrome.slots[board].scale.w, marks);
  const auto &ink = entry.nodes[entry.pointer.wiring].properties.edge;
  for (Whole at = 0; at < NGA::DASHES; at += 1)
    seat[at] = ::seated(marks[at], ink);
}
