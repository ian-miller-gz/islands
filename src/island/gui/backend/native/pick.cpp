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
#include <island/gui/backend/native/kinds.internal.hpp>

namespace NATIVE = GUI::BACKEND::NATIVE;
namespace STATE = GUI::STATE;

static auto cropped(
  const GUI::Chrome &chrome, const GUI::Slot &slot,
  GUI::Position local) -> Flag {
  for (auto above = slot.clipper; above != GUI::PARENTLESS;
       above = chrome.slots[above].clipper) {
    const auto &frame = chrome.slots[above];
    if (
      local.x < frame.x || local.x >= frame.x + frame.w || local.y < frame.y ||
      local.y >= frame.y + frame.h)
      return true;
  }
  return false;
}

auto GUI::BACKEND::pick(Handle document, Position local) -> Whole {
  const auto found = NATIVE::mirrors.find(document);
  if (found == NATIVE::mirrors.end()) return GUI::PARENTLESS;
  const auto &chrome = found->second;
  const auto &nodes = STATE::documents[document].nodes;
  for (auto placed = chrome.order.rbegin(); placed != chrome.order.rend();
       ++placed) {
    const auto index = *placed;
    const auto &node = nodes[index];
    if (!node.live) continue;
    if (!GUI::KINDS::entry(node).interactive && !node.properties.port) continue;
    const auto &slot = chrome.slots[index];
    if (!slot.shown) continue;
    if (
      local.x >= slot.x && local.x < slot.x + slot.w && local.y >= slot.y &&
      local.y < slot.y + slot.h && !::cropped(chrome, slot, local)) {
      return index;
    }
  }
  return GUI::PARENTLESS;
}

auto GUI::BACKEND::extent(Handle document) -> Extent {
  const auto found = NATIVE::mirrors.find(document);
  if (found == NATIVE::mirrors.end()) return {};
  return {found->second.width, found->second.height};
}

auto GUI::BACKEND::measured(Handle document, Whole node) -> Extent {
  const auto found = NATIVE::mirrors.find(document);
  const auto &properties = STATE::documents[document].nodes[node].properties;
  if (found == NATIVE::mirrors.end() || node >= found->second.slots.size())
    return properties.extent;
  const auto &slot = found->second.slots[node];
  return {slot.w, slot.h};
}

auto GUI::BACKEND::origin(Handle document, Whole node) -> Position {
  const auto found = NATIVE::mirrors.find(document);
  const auto &properties = STATE::documents[document].nodes[node].properties;
  if (found == NATIVE::mirrors.end() || node >= found->second.slots.size())
    return properties.position;
  const auto &slot = found->second.slots[node];
  return {slot.x, slot.y};
}
