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
#include <cmath>

namespace {

void seize(GUI::Handle document, GUI::Document &entry, Float x, Float y) {
  auto &pointer = entry.pointer;
  pointer.panning = GUI::decked(document, entry, x, y);
  pointer.swept = false;
  pointer.onset = {x, y};
  const auto hit = GUI::BACKEND::pick(document, {x, y});
  if (
    pointer.panning == GUI::PARENTLESS ||
    GUI::taken(entry, hit, pointer.panning)) {
    pointer.panning = GUI::PARENTLESS;
    pointer.asking =
      hit != GUI::PARENTLESS ? hit : GUI::under(document, entry, x, y);
    return;
  }
  pointer.asking = GUI::boxed(document, entry, pointer.panning, x, y);
  pointer.viewed = entry.nodes[pointer.panning].properties.pan;
}

auto still(GUI::Pointer &pointer, Float x, Float y) -> Flag {
  if (pointer.swept) return false;
  const Float across = x - pointer.onset.x;
  const Float down = y - pointer.onset.y;
  if (std::abs(across) < GUI::THRESHOLD && std::abs(down) < GUI::THRESHOLD)
    return true;
  pointer.swept = true;
  return false;
}

void plant(
  GUI::Handle document, GUI::Document &entry, STRING::Hot id,
  GUI::NGA::Pan wanted, Float x, Float y) {
  GUI::NGA::set(document, id, wanted);
  const auto landed = GUI::NGA::GET::pan(document, id);
  if (landed.x == wanted.x && landed.y == wanted.y) return;
  entry.pointer.viewed = {landed.x, landed.y};
  entry.pointer.onset = {x, y};
}

void carry(
  GUI::Handle document, GUI::Document &entry, Whole deck, Float x, Float y) {
  auto &pointer = entry.pointer;
  if (::still(pointer, x, y)) return;
  const Float across = x - pointer.onset.x;
  const Float down = y - pointer.onset.y;
  const auto &worn = entry.nodes[deck].properties;
  const auto board = GUI::NGA::scaled(document, deck);
  const Float sideways = board.w * (worn.zoom.w > 0.0f ? worn.zoom.w : 1.0f);
  const Float downward = board.h * (worn.zoom.h > 0.0f ? worn.zoom.h : 1.0f);
  const auto id = entry.nodes[deck].id;
  ::plant(
    document, entry, id.c_str(),
    GUI::NGA::Pan{
      pointer.viewed.x - across / sideways, pointer.viewed.y - down / downward},
    x, y);
}

auto owned(const GUI::Pointer &pointer) -> Flag {
  return pointer.banding != GUI::PARENTLESS ||
         pointer.cutting != GUI::PARENTLESS ||
         pointer.wiring != GUI::PARENTLESS;
}

void answer(GUI::Document &entry, Whole deck) {
  auto &pointer = entry.pointer;
  if (pointer.swept) return;
  const auto box = pointer.asking;
  const Flag stands = box != GUI::PARENTLESS && entry.nodes[box].live;
  const Flag decked = deck != GUI::PARENTLESS;
  if (!decked && !stands) return;
  pointer.ask = {
    decked ? entry.nodes[deck].id : STRING::Cold(),
    stands ? entry.nodes[box].id : STRING::Cold(), pointer.onset.x,
    pointer.onset.y, true};
}

}  // namespace

void GUI::sweep(Handle document, Float x, Float y, Flag down) {
  auto &entry = STATE::documents[document];
  auto &pointer = entry.pointer;
  if (down && !pointer.panned) ::seize(document, entry, x, y);
  const auto deck = pointer.panning;
  const Flag released = !down && pointer.panned;
  pointer.panned = down;
  if (!down) pointer.panning = PARENTLESS;
  if (deck != PARENTLESS && !entry.nodes[deck].live) return;
  if (released)
    ::answer(entry, deck);
  else if (!down)
    return;
  else if (deck == PARENTLESS)
    ::still(pointer, x, y);
  else if (!::owned(pointer))
    ::carry(document, entry, deck, x, y);
}
