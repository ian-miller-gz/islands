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
#include <island/gui/nga.internal.hpp>
#include <algorithm>

static auto stacked(const GUI::Document &entry, Whole index) -> Flag {
  return GUI::BACKEND::NATIVE::stacked(entry, index);
}

static auto diverged(
  const GUI::Document &entry, const GUI::Chrome &chrome, Whole index) -> Flag {
  const auto &properties = entry.nodes[index].properties;
  const auto parent = entry.nodes[index].parent;
  const auto &slot = chrome.slots[index];
  const Float z =
    properties.z + (parent == GUI::PARENTLESS ? 0.0f : chrome.slots[parent].z);
  if (
    properties.visible != slot.visible || z != slot.z ||
    properties.atlas != slot.atlas || properties.clip != slot.clip) {
    return true;
  }
  return ::stacked(entry, index) &&
         (properties.position.x != slot.offset.x ||
          properties.position.y != slot.offset.y ||
          properties.extent.w != slot.w ||
          GUI::BACKEND::NATIVE::measure(
            entry.nodes[index], slot, properties.extent.h) != slot.h);
}

auto GUI::BACKEND::NATIVE::rewrite(
  Handle document, Document &entry, Chrome &chrome) -> Status {
  for (const Whole index : entry.dirties)
    if (entry.nodes[index].live && ::diverged(entry, chrome, index)) return 1;
  const Whole stamp = chrome.stamp += 1;
  Vector<Whole> decks;
  if (const auto held = NGA::pending(entry); held != PARENTLESS)
    decks.push_back(held);
  if (const auto caught = NGA::banding(entry);
      caught != PARENTLESS &&
      std::find(decks.begin(), decks.end(), caught) == decks.end())
    decks.push_back(caught);
  for (const Whole index : ordering(entry)) {
    const auto &node = entry.nodes[index];
    const Whole mate = referent(entry, index);
    const Flag carried =
      node.dirty ||
      (node.parent != PARENTLESS && chrome.touched[node.parent] == stamp) ||
      (mate != PARENTLESS && chrome.touched[mate] == stamp);
    if (!carried) continue;
    chrome.touched[index] = stamp;
    if (::stacked(entry, index) && chrome.touched[node.parent] != stamp) {
      chrome.slots[index].cursor = 0.0f;
    } else {
      resolve(entry, chrome, index);
      if (node.row == KINDS::GRAPH) NGA::settle(document, index);
    }
    if (chrome.slots[index].shown) emit(entry, chrome, index);
    const auto deck =
      entry.nodes[index].row == KINDS::GRAPH ? index : board(entry, index);
    if (deck == PARENTLESS) continue;
    if (std::find(decks.begin(), decks.end(), deck) == decks.end())
      decks.push_back(deck);
  }
  for (const auto deck : decks) wire(entry, chrome, deck);
  return 0;
}
