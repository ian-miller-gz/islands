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

auto held(
  GUI::Handle document, const GUI::Document &entry, Whole index, Flag across,
  Float wanted) -> Float {
  const auto &node = entry.nodes[index];
  const auto room = node.parent == GUI::PARENTLESS
                      ? GUI::BACKEND::extent(document)
                      : GUI::BACKEND::measured(document, node.parent);
  const Float scale = ::along(across, GUI::NGA::scaled(document, index));
  const Float span = ::along(across, GUI::BACKEND::measured(document, index));
  const Float high = std::max(0.0f, (::along(across, room) - span) / scale);
  const Float near = ::share(node.properties.anchor, across);
  return std::clamp(wanted, -high * near, high * (1.0f - near));
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
  record(entry, index, Event::MOVED);
  pointer.moves.push_back(entry.nodes[index].id);
}
