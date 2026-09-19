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

static void restyle(GUI::Handle document, Whole index, GUI::State state) {
  auto &node = STATE::documents[document].nodes[index];
  if (!node.live || node.state == state) return;
  node.state = state;
  GUI::mark(document, index);
}

void GUI::record(Document &entry, Whole index, Event::Kind kind) {
  entry.pointer.events.push_back({entry.nodes[index].id, kind});
}

static void unfold(GUI::Handle document, GUI::Document &entry, Whole hit) {
  const auto &node = entry.nodes[hit];
  if (node.kind != "detail") return;
  GUI::set(document, node.id.c_str(), GUI::Opening{!node.properties.open});
}

void GUI::grab(Document &entry, Whole hit, Float x, Float y) {
  auto &pointer = entry.pointer;
  pointer.grab = {x, y};
  pointer.dragged = false;
  const auto &properties = entry.nodes[hit].properties;
  pointer.grabbed = properties.value;
  pointer.standing = properties.position;
}

static void squeeze(
  GUI::Handle document, GUI::Document &entry, Whole hit, Flag down, Float x,
  Float y) {
  auto &pointer = entry.pointer;
  if (down && !pointer.down) {
    pointer.landed = true;
    if (hit != GUI::PARENTLESS) {
      pointer.pressed = hit;
      GUI::grab(entry, hit, x, y);
      GUI::record(entry, hit, GUI::Event::PRESSED);
    }
    GUI::hitch(document, entry, hit, x, y);
    GUI::stamp(document, entry, hit, x, y);
  }
  if (down) {
    GUI::drag(document, entry, x, y);
    GUI::trace(document, entry, x);
    GUI::shift(document, entry, x, y);
    GUI::slide(document, entry, x, y);
    GUI::trail(document, entry, x, y);
    GUI::stroke(document, entry, x, y);
  }
  if (!down && pointer.down) {
    if (pointer.stroking != GUI::PARENTLESS) {
      pointer.lifting = pointer.stroking;
      pointer.lift = pointer.stroke;
    }
    pointer.stroking = GUI::PARENTLESS;
    pointer.stroke = {};
    const auto held = pointer.pressed;
    if (
      held != GUI::PARENTLESS && !pointer.dragged &&
      (held == hit || held == pointer.wiring)) {
      GUI::record(entry, held, GUI::Event::CLICKED);
      pointer.clicks.push_back(entry.nodes[held].id);
      ::unfold(document, entry, held);
      GUI::select(document, entry, held);
      const auto deck = GUI::board(entry, held);
      if (deck == GUI::PARENTLESS || !entry.nodes[deck].properties.banded)
        GUI::NGA::select(document, entry, held);
      GUI::flip(document, entry, held);
      GUI::restore(document, entry, held);
      GUI::seat(document, entry, held);
    }
    GUI::moor(document, entry);
    GUI::fasten(document, entry, hit);
    pointer.pressed = GUI::PARENTLESS;
    pointer.dragged = false;
  }
}

void GUI::point(Handle document, Float x, Float y, Flag inside, Flag down) {
  if (!live(document)) return;
  auto &entry = STATE::documents[document];
  auto &pointer = entry.pointer;
  pointer.spot = {x, y};
  const Flag captured =
    down && pointer.dragged && pointer.pressed != PARENTLESS &&
    entry.nodes[pointer.pressed].live && pointer.wiring == PARENTLESS;
  const auto hit = !inside    ? PARENTLESS
                   : captured ? pointer.pressed
                              : BACKEND::pick(document, {x, y});
  if (hit != pointer.hovered) {
    if (pointer.hovered != PARENTLESS && entry.nodes[pointer.hovered].live)
      record(entry, pointer.hovered, Event::EXITED);
    if (hit != PARENTLESS) record(entry, hit, Event::ENTERED);
  }
  ::squeeze(document, entry, hit, down, x, y);
  if (pointer.hovered != PARENTLESS && pointer.hovered != hit)
    ::restyle(document, pointer.hovered, REST);
  if (hit != PARENTLESS)
    ::restyle(document, hit, down && pointer.pressed == hit ? PRESS : HOVER);
  pointer.hovered = hit;
  pointer.down = down;
}
