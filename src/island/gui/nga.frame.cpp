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
#include <island/gui/nga.hpp>
#include <island/gui/events.internal.hpp>
#include <island/gui/gui.internal.hpp>

auto GUI::NGA::GET::wiring(Handle document) -> STRING::Cold {
  if (!live(document)) return "";
  const auto &entry = STATE::documents[document];
  const auto held = entry.pointer.wiring;
  if (held == PARENTLESS || !entry.nodes[held].live) return "";
  return entry.nodes[held].id;
}

auto GUI::NGA::GET::aimed(Handle document) -> STRING::Cold {
  if (!live(document)) return "";
  const auto &entry = STATE::documents[document];
  const auto &pointer = entry.pointer;
  Whole found = PARENTLESS;
  if (pointer.wiring != PARENTLESS)
    found = pointer.snapped;
  else if (pointer.pressed == PARENTLESS && !pointer.down)
    found =
      aim(document, entry, pointer.hovered, pointer.spot.x, pointer.spot.y);
  if (found == PARENTLESS || !entry.nodes[found].live) return "";
  return entry.nodes[found].id;
}

auto GUI::NGA::GET::joined(Handle document) -> Vector<Link> {
  if (!live(document)) return {};
  return STATE::documents[document].pointer.joins;
}

auto GUI::NGA::GET::cut(Handle document) -> Vector<Link> {
  if (!live(document)) return {};
  return STATE::documents[document].pointer.cuts;
}

auto GUI::NGA::GET::asked(Handle document) -> Ask {
  if (!live(document)) return {};
  return STATE::documents[document].pointer.ask;
}

auto GUI::NGA::GET::hovered(Handle document, STRING::Hot board) -> Ask {
  Whole deck = PARENTLESS;
  if (find(document, board, deck) != 0) return {};
  const auto &entry = STATE::documents[document];
  const auto &pointer = entry.pointer;
  const Flag away = STATE::claimed != NONE && STATE::claimed != document;
  if (away || !pointer.inside || pointer.down || pointer.pressed != PARENTLESS)
    return {};
  const auto [x, y] = pointer.spot;
  if (decked(document, entry, x, y) != deck) return {};
  if (taken(entry, pointer.hovered, deck)) return {};
  const auto box = boxed(document, entry, deck, x, y);
  const auto at = aboard(document, entry, deck, x, y);
  return {
    entry.nodes[deck].id,
    box == PARENTLESS ? STRING::Cold() : entry.nodes[box].id, at.x, at.y, true};
}
