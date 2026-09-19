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
#include <island/graphics/text/text.hpp>
#include <cmath>

namespace NGA = GUI::NGA;
namespace NATIVE = GUI::BACKEND::NATIVE;

namespace {

auto around(const GUI::Document &entry, const GUI::Chrome &chrome, Whole index)
  -> NGA::Span {
  const auto &slot = chrome.slots[GUI::box(entry, index)];
  return {slot.x, slot.y, slot.w, slot.h};
}

void draw(const NGA::Span *run, const GFX::Color &ink, GUI::Element *seat) {
  const Float held = GFX::TEXT::GET::scale();
  const Float scale = held > 0.0f ? held : 1.0f;
  for (Whole at = 0; at < NGA::SEGMENTS; at += 1) {
    const Float x = std::round(run[at].x * scale);
    const Float y = std::round(run[at].y * scale);
    const Float w = std::max(std::round(run[at].w * scale), 1.0f);
    const Float h = std::max(std::round(run[at].h * scale), 1.0f);
    seat[at] = {x / scale, y / scale, w / scale, h / scale, GUI::FULL, ink};
  }
}

void join(
  const GUI::Document &entry, const GUI::Chrome &chrome, const NGA::Link &link,
  Float scale, GUI::Element *seat) {
  const auto source = entry.ids.find(link.from);
  const auto sink = entry.ids.find(link.to);
  if (source == entry.ids.end() || sink == entry.ids.end()) {
    for (Whole at = 0; at < NGA::SEGMENTS; at += 1) seat[at] = {};
    return;
  }
  NGA::Span run[NGA::SEGMENTS];
  NGA::route(
    {NATIVE::centre(chrome.slots[source->second]),
     NGA::rank(entry, source->second), ::around(entry, chrome, source->second)},
    {NATIVE::centre(chrome.slots[sink->second]), NGA::rank(entry, sink->second),
     ::around(entry, chrome, sink->second)},
    NGA::THICKNESS * scale, run);
  const auto &worn = entry.nodes[source->second].properties;
  ::draw(
    run,
    link.style.empty() ? worn.edge : GUI::worn(entry, link.style.c_str()).edge,
    seat);
}

}  // namespace

auto GUI::KINDS::seats(const Document &entry, Whole index) -> Whole {
  const auto quads = KINDS::entry(entry.nodes[index]).quads;
  if (entry.nodes[index].row != KINDS::GRAPH) return quads;
  const Whole loose = NGA::pending(entry) == index ? 1 : 0;
  Whole wires = loose;
  for (const auto &link : entry.links)
    if (GUI::board(entry, link) == index) wires += 1;
  const Whole marquee = NGA::banding(entry) == index ? 1 : 0;
  return quads + wires * NGA::SEGMENTS + loose * NGA::DASHES + marquee;
}

void GUI::BACKEND::NATIVE::wire(Document &entry, Chrome &chrome, Whole index) {
  if (entry.nodes[index].row != KINDS::GRAPH) return;
  const auto &slot = chrome.slots[index];
  if (!slot.shown) return;
  auto *elements = GFX::BUFFERS::GET::data<Element>(chrome.buffer);
  auto *seat = elements + slot.first + KINDS::entry(entry.nodes[index]).quads;
  for (const auto &link : entry.links) {
    if (GUI::board(entry, link) != index) continue;
    ::join(entry, chrome, link, slot.scale.w, seat);
    seat += NGA::SEGMENTS;
    STATE::writes += NGA::SEGMENTS;
  }
  if (NGA::pending(entry) == index) {
    const auto held = entry.pointer.wiring;
    const auto loose = entry.pointer.reach;
    NGA::Span run[NGA::SEGMENTS];
    NGA::route(
      {NATIVE::centre(chrome.slots[held]), NGA::rank(entry, held),
       ::around(entry, chrome, held)},
      {loose, 0, {loose.x, loose.y, 0.0f, 0.0f}}, NGA::THICKNESS * slot.scale.w,
      run);
    ::draw(run, entry.nodes[held].properties.edge, seat);
    seat += NGA::SEGMENTS;
    STATE::writes += NGA::SEGMENTS;
    NATIVE::guide(entry, chrome, index, seat);
    seat += NGA::DASHES;
  }
  if (NGA::banding(entry) != index) return;
  const auto caught = NGA::band(entry);
  seat[0] = {caught.x, caught.y,  caught.w,
             caught.h, GUI::FULL, entry.nodes[index].properties.hover};
  STATE::writes += 1;
}
