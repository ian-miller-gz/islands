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
#include <island/gui/backend/native/native.hpp>
#include <island/graphics/text/text.hpp>

namespace NATIVE = GUI::BACKEND::NATIVE;
namespace STATE = GUI::STATE;

auto GUI::BACKEND::caret(Handle document, Whole node, Float x) -> Whole {
  constexpr Float HALF = 0.5f;
  const auto &entry = STATE::documents[document].nodes[node];
  const auto end = entry.properties.text.size();
  const auto found = NATIVE::mirrors.find(document);
  if (found == NATIVE::mirrors.end() || node >= found->second.slots.size())
    return end;
  const auto &slot = found->second.slots[node];
  const Float advance = GFX::TEXT::GET::advance(NATIVE::lettered(entry, slot));
  if (advance <= 0.0f) return end;
  const Float from = slot.x + entry.properties.pad * slot.scale.w;
  const Float across = (x - from) / advance;
  if (across <= 0.0f) return 0;
  const auto landed = static_cast<Whole>(across + HALF);
  return landed < end ? landed : end;
}
