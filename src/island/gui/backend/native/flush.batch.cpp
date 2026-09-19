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
#include <island/graphics/backend/pipelines.hpp>
#include <island/graphics/backend/resources.hpp>

static auto pipeline() -> GFX::Handle {
  return GFX::PIPELINES::create(
    {.vertex =
       {.path = String(GUI::SHADERS) + ".vertex",
        .uniforms = {sizeof(GFX::SHADERS::Camera)},
        .buffers = 1},
     .fragment = {.path = String(GUI::SHADERS) + ".fragment", .samplers = 1}});
}

static constexpr Whole HEADROOM = 2;

static void size(GUI::Chrome &chrome, Whole needed) {
  if (needed <= chrome.limit) return;
  const auto room = needed + needed / HEADROOM;
  if (chrome.buffer != GFX::NONE) GFX::BUFFERS::destroy(chrome.buffer);
  chrome.buffer = GFX::BUFFERS::create(room * sizeof(GUI::Element));
  chrome.limit = room;
}

void GUI::BACKEND::NATIVE::batch(Document &entry, Chrome &chrome) {
  chrome.runs.clear();
  Whole seats = 0;
  for (const auto index : chrome.order) {
    const auto &node = entry.nodes[index];
    const auto &kind = KINDS::entry(node);
    const auto claimed = KINDS::seats(entry, index);
    if (!claimed) continue;
    const auto atlas = kind.textured && node.properties.atlas != UNBOUND
                         ? node.properties.atlas
                         : STATE::white;
    const auto z = chrome.slots[index].z;
    const auto clipper = chrome.slots[index].clipper;
    if (
      chrome.runs.empty() || chrome.runs.back().atlas != atlas ||
      chrome.runs.back().z != z || chrome.runs.back().clipper != clipper)
      chrome.runs.push_back({atlas, chrome.slots[index].first, 0, z, clipper});
    chrome.runs.back().count += claimed;
    seats += claimed;
    if (!chrome.pipelines.contains(atlas))
      chrome.pipelines[atlas] = ::pipeline();
  }
  ::size(chrome, seats);
}
