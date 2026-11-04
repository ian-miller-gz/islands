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
#include <island/gui/nga.internal.hpp>
#include <island/window/backend.hpp>
#include <algorithm>
#include <cmath>

namespace {

const Whole GRIP = GUI::BACKEND::row("grip");

auto along(Flag across, GUI::Position pair) -> Float {
  return across ? pair.x : pair.y;
}

auto along(Flag across, GUI::Extent pair) -> Float {
  return across ? pair.w : pair.h;
}

auto share(GUI::Anchor anchor, Flag across) -> Float {
  if (anchor == GUI::CENTER) return 0.5f;
  const Flag far = across
                     ? (anchor == GUI::NORTHEAST || anchor == GUI::SOUTHEAST)
                     : (anchor == GUI::SOUTHWEST || anchor == GUI::SOUTHEAST);
  return far ? 1.0f : 0.0f;
}

auto room(GUI::Handle document, const GUI::Document &entry, Whole index)
  -> GUI::Extent {
  const auto parent = entry.nodes[index].parent;
  return parent == GUI::PARENTLESS ? GUI::BACKEND::extent(document)
                                   : GUI::BACKEND::measured(document, parent);
}

auto held(
  GUI::Handle document, const GUI::Document &entry, Whole index, Flag across,
  Float wanted) -> Float {
  const auto &node = entry.nodes[index];
  const Float scale = ::along(across, GUI::NGA::scaled(document, index));
  const Float span = ::along(across, GUI::BACKEND::measured(document, index));
  const Float high = std::max(
    0.0f, (::along(across, ::room(document, entry, index)) - span) / scale);
  const Float near = ::share(node.properties.anchor, across);
  const auto plate = GUI::whole(document, entry, index);
  const Float lead = ::along(across, GUI::BACKEND::origin(document, index)) -
                     ::along(across, plate.at);
  const Float trail = ::along(across, plate.span) - lead - span;
  const Float low = -high * near + lead / scale;
  const Float top = high * (1.0f - near) - trail / scale;
  return low > top ? low : std::clamp(wanted, low, top);
}

void confine(GUI::Handle document, const GUI::Document &entry, Whole index) {
  const auto &pointer = entry.pointer;
  if (!pointer.placed) return;
  const auto &rect = pointer.placement;
  const auto extent = GUI::BACKEND::extent(document);
  if (rect.w <= 0.0f || rect.h <= 0.0f || extent.w <= 0.0f || extent.h <= 0.0f)
    return;
  const auto &grant = entry.nodes[index].properties;
  const auto plate = GUI::whole(document, entry, index);
  const auto corner = GUI::BACKEND::origin(document, index);
  const auto box = ::room(document, entry, index);
  const auto parent = entry.nodes[index].parent;
  const GUI::Position home = parent == GUI::PARENTLESS
                               ? GUI::Position{}
                               : GUI::BACKEND::origin(document, parent);
  Float low[2] = {0.0f, 0.0f}, high[2] = {extent.w, extent.h};
  const Flag travels[2] = {
    grant.across || grant.free, !grant.across || grant.free};
  const Float origin[2] = {home.x, home.y};
  const Float width[2] = {box.w, box.h};
  const Float held[2] = {pointer.grab.x - corner.x, pointer.grab.y - corner.y};
  const Float lead[2] = {corner.x - plate.at.x, corner.y - plate.at.y};
  const Float span[2] = {plate.span.w, plate.span.h};
  for (Whole axis = 0; axis < 2; axis += 1) {
    if (!travels[axis]) continue;
    low[axis] = origin[axis] + lead[axis] + held[axis];
    high[axis] = std::max(
      low[axis],
      origin[axis] + width[axis] - span[axis] + lead[axis] + held[axis]);
  }
  const Float across = rect.w / extent.w;
  const Float down = rect.h / extent.h;
  const auto grid = [](Float length) {
    return static_cast<Whole>(std::lround(std::max(0.0f, length)));
  };
  WINDOW::confine(
    grid(rect.x + low[0] * across), grid(rect.y + low[1] * down),
    grid((high[0] - low[0]) * across) + 1, grid((high[1] - low[1]) * down) + 1);
}

auto travelled(
  GUI::Handle document, const GUI::Document &entry, Whole index, Flag across,
  Float pull) -> Float {
  const Float scale = ::along(across, GUI::NGA::scaled(document, index));
  const Float wanted = ::along(across, entry.pointer.standing) + pull / scale;
  return ::held(document, entry, index, across, wanted);
}

}  // namespace

void GUI::slide(Handle document, Document &entry, Float x, Float y) {
  auto &pointer = entry.pointer;
  const auto index = pointer.pressed;
  if (index == PARENTLESS || !entry.nodes[index].live) return;
  if (entry.nodes[index].row != ::GRIP) return;
  const auto &grant = entry.nodes[index].properties;
  const Position pull = {x - pointer.grab.x, y - pointer.grab.y};
  const Float measured =
    grant.free ? (std::abs(pull.x) > std::abs(pull.y) ? pull.x : pull.y)
               : ::along(grant.across, pull);
  if (!pointer.dragged && std::abs(measured) < THRESHOLD) return;
  if (!pointer.dragged) {
    pointer.armed = PARENTLESS;
    carry(document, entry, index, true);
    ::confine(document, entry, index);
  }
  pointer.dragged = true;
  auto landed = grant.position;
  if (grant.across || grant.free)
    landed.x = ::travelled(document, entry, index, true, pull.x);
  if (!grant.across || grant.free)
    landed.y = ::travelled(document, entry, index, false, pull.y);
  GUI::set(document, entry.nodes[index].id.c_str(), landed);
}

void GUI::moor(Handle document, Document &entry) {
  auto &pointer = entry.pointer;
  const auto index = pointer.pressed;
  if (index == PARENTLESS || !entry.nodes[index].live) return;
  if (entry.nodes[index].row != ::GRIP || !pointer.dragged) return;
  carry(document, entry, index, false);
  WINDOW::release();
  record(entry, index, Event::MOVED);
  pointer.moves.push_back(entry.nodes[index].id);
}
