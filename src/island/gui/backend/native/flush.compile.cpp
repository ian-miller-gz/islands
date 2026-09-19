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
#include <island/graphics/backend/resources.hpp>
#include <algorithm>

namespace STATE = GUI::STATE;

static constexpr Float FACTORS[][2] = {
  {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {0.5f, 0.5f}};

static void stack(
  GUI::Document &entry, GUI::Chrome &chrome, Whole index, const Float axis[2]) {
  const auto &node = entry.nodes[index];
  auto &above = chrome.slots[node.parent];
  auto &slot = chrome.slots[index];
  const auto scale = slot.scale;
  slot.x =
    above.x + axis[0] * above.cursor + node.properties.position.x * scale.w;
  slot.y =
    above.y + axis[1] * above.cursor + node.properties.position.y * scale.h;
  above.cursor += axis[0] * slot.w + axis[1] * slot.h +
                  entry.nodes[node.parent].properties.pad *
                    (axis[0] != 0.0f ? scale.w : scale.h);
}

static auto zoomed(const GUI::Properties &properties) -> GUI::Extent {
  const auto scale = properties.zoom;
  return {scale.w > 0.0f ? scale.w : 1.0f, scale.h > 0.0f ? scale.h : 1.0f};
}

static void aboard(GUI::Document &entry, GUI::Chrome &chrome, Whole index) {
  const auto &node = entry.nodes[index];
  const auto &above = chrome.slots[node.parent];
  const auto &view = entry.nodes[node.parent].properties;
  const auto pinned = node.properties.pinned;
  auto &slot = chrome.slots[index];
  slot.x =
    pinned == GUI::NGA::Pinned::ACROSS
      ? above.x + node.properties.position.x
      : above.x + (node.properties.position.x - view.pan.x) * slot.scale.w;
  slot.y =
    pinned == GUI::NGA::Pinned::DOWN
      ? above.y + node.properties.position.y
      : above.y + (node.properties.position.y - view.pan.y) * slot.scale.h;
}

static void anchor(
  const GUI::Properties &properties, const GUI::Slot &above, GUI::Slot &slot,
  const Float factor[2]) {
  const auto pins = properties.pins;
  const auto scale = slot.scale;
  if ((pins & GUI::EDGE::LEFT) && (pins & GUI::EDGE::RIGHT)) {
    slot.x = above.x + properties.position.x * scale.w;
    slot.w = above.w - (properties.position.x + properties.extent.w) * scale.w;
  } else {
    slot.x = above.x + (above.w - slot.w) * factor[0] +
             properties.position.x * scale.w;
  }
  if ((pins & GUI::EDGE::TOP) && (pins & GUI::EDGE::BOTTOM)) {
    slot.y = above.y + properties.position.y * scale.h;
    slot.h = above.h - (properties.position.y + properties.extent.h) * scale.h;
  } else {
    slot.y = above.y + (above.h - slot.h) * factor[1] +
             properties.position.y * scale.h;
  }
}

static auto sided(
  Flag after, Float corner, Float span, Float air, Float mine,
  Float page) -> Float {
  const Float beyond = corner + span + air;
  const Float before = corner - air - mine;
  const Float wanted = after ? beyond : before;
  const Float other = after ? before : beyond;
  const Flag hangs = wanted < 0.0f || wanted + mine > page;
  const Flag holds = other >= 0.0f && other + mine <= page;
  return GUI::fit(hangs && holds ? other : wanted, mine, 0.0f, page);
}

static void beside(
  GUI::Document &entry, GUI::Chrome &chrome, Whole index, Whole referent,
  const Float factor[2]) {
  const auto &properties = entry.nodes[index].properties;
  const auto &mate = chrome.slots[referent];
  auto &slot = chrome.slots[index];
  const auto scale = slot.scale;
  const auto side = static_cast<GUI::Side>(properties.side);
  const Flag down = side == GUI::BELOW || side == GUI::ABOVE;
  const Float air = properties.air * (down ? scale.h : scale.w);
  const Flag after = side == GUI::BELOW || side == GUI::EAST;
  if (down) {
    slot.x = GUI::fit(
      mate.x + (mate.w - slot.w) * factor[0] + properties.position.x * scale.w,
      slot.w, 0.0f, chrome.width);
    slot.y = ::sided(after, mate.y, mate.h, air, slot.h, chrome.height);
    return;
  }
  slot.x = ::sided(after, mate.x, mate.w, air, slot.w, chrome.width);
  slot.y = GUI::fit(
    mate.y + (mate.h - slot.h) * factor[1] + properties.position.y * scale.h,
    slot.h, 0.0f, chrome.height);
}

auto GUI::BACKEND::NATIVE::referent(const Document &entry, Whole index)
  -> Whole {
  const auto &beside = entry.nodes[index].properties.beside;
  if (beside.empty()) return PARENTLESS;
  const auto found = entry.ids.find(String(beside));
  return found == entry.ids.end() ? PARENTLESS : found->second;
}

auto GUI::BACKEND::NATIVE::stacked(const Document &entry, Whole index) -> Flag {
  const auto parent = entry.nodes[index].parent;
  if (parent == PARENTLESS || entry.nodes[index].rim) return false;
  return KINDS::stacked(KINDS::entry(entry.nodes[parent]));
}

void GUI::BACKEND::NATIVE::resolve(
  Document &entry, Chrome &chrome, Whole index) {
  const auto &node = entry.nodes[index];
  const auto &properties = node.properties;
  auto &slot = chrome.slots[index];
  const Extent identity = {1.0f, 1.0f};
  const Extent inherited =
    node.parent == PARENTLESS ? identity : chrome.slots[node.parent].scale;
  const Extent view =
    node.parent != PARENTLESS && entry.nodes[node.parent].row == KINDS::GRAPH
      ? ::zoomed(entry.nodes[node.parent].properties)
      : identity;
  slot.scale = {inherited.w * view.w, inherited.h * view.h};
  if (properties.pinned == NGA::Pinned::ACROSS) slot.scale.w = 1.0f;
  if (properties.pinned == NGA::Pinned::DOWN) slot.scale.h = 1.0f;
  slot.w = properties.extent.w * slot.scale.w;
  slot.h = properties.extent.h * slot.scale.h;
  slot.cursor = 0.0f;
  slot.offset = properties.position;
  slot.visible = properties.visible;
  slot.z = properties.z +
           (node.parent == PARENTLESS ? 0.0f : chrome.slots[node.parent].z);
  slot.atlas = properties.atlas;
  slot.clip = properties.clip;
  slot.clipper = node.parent == PARENTLESS ? PARENTLESS
                 : entry.nodes[node.parent].properties.clip
                   ? node.parent
                   : chrome.slots[node.parent].clipper;
  const Whole mate = referent(entry, index);
  if (mate != PARENTLESS) {
    slot.h = measure(node, slot, slot.h);
    return ::beside(entry, chrome, index, mate, FACTORS[properties.anchor]);
  }
  if (node.parent != PARENTLESS) {
    if (entry.nodes[node.parent].row == KINDS::GRAPH) {
      slot.h = measure(node, slot, slot.h);
      return ::aboard(entry, chrome, index);
    }
    if (stacked(entry, index)) {
      slot.h = measure(node, slot, slot.h);
      return ::stack(
        entry, chrome, index, KINDS::entry(entry.nodes[node.parent]).stack);
    }
  }
  const Slot root = {.w = chrome.width, .h = chrome.height};
  const auto &above =
    node.parent == PARENTLESS ? root : chrome.slots[node.parent];
  ::anchor(properties, above, slot, FACTORS[properties.anchor]);
  slot.h = measure(node, slot, slot.h);
}

void GUI::BACKEND::NATIVE::emit(Document &entry, Chrome &chrome, Whole index) {
  const auto &slot = chrome.slots[index];
  chrome.lines[index] = fold(entry.nodes[index], slot);
  const auto &kind = KINDS::entry(entry.nodes[index]);
  if (!kind.quads) return;
  auto *elements = GFX::BUFFERS::GET::data<Element>(chrome.buffer);
  kind.compose(entry.nodes[index], slot, elements + slot.first);
  STATE::writes += kind.quads;
}

auto GUI::BACKEND::NATIVE::ordering(const Document &entry) -> Vector<Whole> {
  Vector<Flag> seen(entry.nodes.size(), false);
  Vector<Whole> sequence;
  sequence.reserve(entry.nodes.size());
  const auto walk = [&](Whole index, auto &again) -> void {
    if (seen[index] || !entry.nodes[index].live) return;
    seen[index] = true;
    if (entry.nodes[index].parent != PARENTLESS)
      again(entry.nodes[index].parent, again);
    const Whole mate = referent(entry, index);
    if (mate != PARENTLESS) again(mate, again);
    sequence.push_back(index);
  };
  for (Whole index = 0; index < entry.nodes.size(); index += 1)
    walk(index, walk);
  return sequence;
}

static void settle(
  GUI::Handle document, GUI::Document &entry, GUI::Chrome &chrome,
  Whole index) {
  using namespace GUI::BACKEND::NATIVE;
  const auto &node = entry.nodes[index];
  const Flag above =
    node.parent == GUI::PARENTLESS || chrome.slots[node.parent].shown;
  if (!above) {
    chrome.slots[index] = chrome.slots[node.parent];
    chrome.slots[index].shown = false;
    return;
  }
  resolve(entry, chrome, index);
  chrome.slots[index].shown = node.properties.visible;
  if (node.row == GUI::KINDS::GRAPH) GUI::NGA::settle(document, index);
}

static void band(const GUI::Document &entry, GUI::Chrome &chrome) {
  for (Whole index = 0; index < entry.nodes.size(); index += 1)
    if (entry.nodes[index].live && chrome.slots[index].shown)
      chrome.order.push_back(index);
  std::stable_sort(
    chrome.order.begin(), chrome.order.end(), [&chrome](Whole a, Whole b) {
      return chrome.slots[a].z < chrome.slots[b].z;
    });
}

void GUI::BACKEND::NATIVE::compile(
  Handle document, Document &entry, Chrome &chrome) {
  chrome.slots.assign(entry.nodes.size(), {});
  chrome.lines.assign(entry.nodes.size(), {});
  chrome.touched.assign(entry.nodes.size(), 0);
  chrome.stamp = 0;
  chrome.order.clear();
  for (const Whole index : ordering(entry))
    ::settle(document, entry, chrome, index);
  ::band(entry, chrome);
  Whole seats = 0;
  for (const auto index : chrome.order) {
    chrome.slots[index].first = seats;
    seats += KINDS::seats(entry, index);
  }
  batch(entry, chrome);
  for (const auto index : chrome.order) emit(entry, chrome, index);
  for (const auto index : chrome.order) wire(entry, chrome, index);
}
