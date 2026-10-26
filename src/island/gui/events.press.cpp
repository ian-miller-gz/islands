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
#include <island/gui/nga.internal.hpp>
#include <island/gui/backend.hpp>

namespace {

auto aboard(
  GUI::Handle document, const GUI::Document &entry, Whole deck, Float x,
  Float y) -> GUI::Position {
  const auto &worn = entry.nodes[deck].properties;
  const auto board = GUI::NGA::scaled(document, deck);
  const Float across = board.w * (worn.zoom.w > 0.0f ? worn.zoom.w : 1.0f);
  const Float down = board.h * (worn.zoom.h > 0.0f ? worn.zoom.h : 1.0f);
  const auto corner = GUI::BACKEND::origin(document, deck);
  return {
    worn.pan.x + (x - corner.x) / across, worn.pan.y + (y - corner.y) / down};
}

}  // namespace

void GUI::stamp(Handle document, Document &entry, Whole hit, Float x, Float y) {
  const auto deck = decked(document, entry, x, y);
  if (deck == PARENTLESS) return;
  if (taken(entry, hit, deck)) return;
  const auto box = boxed(document, entry, deck, x, y);
  const auto at = ::aboard(document, entry, deck, x, y);
  auto &pointer = entry.pointer;
  pointer.pressing = deck;
  pointer.press = {
    entry.nodes[deck].id,
    box == PARENTLESS ? STRING::Cold() : entry.nodes[box].id, at.x, at.y, true};
  pointer.stroking = deck;
  pointer.stroke = pointer.press;
}

void GUI::stroke(Handle document, Document &entry, Float x, Float y) {
  auto &pointer = entry.pointer;
  const auto deck = pointer.stroking;
  if (deck == PARENTLESS || !entry.nodes[deck].live) return;
  const auto at = ::aboard(document, entry, deck, x, y);
  pointer.stroke.x = at.x;
  pointer.stroke.y = at.y;
}

auto GUI::NGA::GET::pressed(Handle document, STRING::Hot id) -> Ask {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  auto &pointer = STATE::documents[document].pointer;
  if (pointer.pressing != node) return {};
  const auto press = pointer.press;
  pointer.press = {};
  pointer.pressing = PARENTLESS;
  return press;
}

auto GUI::NGA::GET::stroked(Handle document, STRING::Hot id) -> Ask {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  const auto &pointer = STATE::documents[document].pointer;
  if (pointer.stroking != node) return {};
  return pointer.stroke;
}

auto GUI::NGA::GET::lifted(Handle document, STRING::Hot id) -> Ask {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return {};
  auto &pointer = STATE::documents[document].pointer;
  if (pointer.lifting != node) return {};
  const auto lift = pointer.lift;
  pointer.lift = {};
  pointer.lifting = PARENTLESS;
  return lift;
}
