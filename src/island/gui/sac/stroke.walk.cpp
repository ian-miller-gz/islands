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
#include <algorithm>
#include <cmath>

#include <island/gui/events.internal.hpp>
#include <island/gui/stroke.internal.hpp>

namespace STROKE = GUI::SAC::STROKE;

namespace {

auto celled(const GUI::Extent &cell, Float x, Float y) -> STROKE::Cell {
  return {Integer(std::floor(x / cell.w)), Integer(std::floor(y / cell.h))};
}

auto marked(const GUI::Extent &cell, Float x, Float y) -> STROKE::Mark {
  return {{x, y}, ::celled(cell, x, y)};
}

void lay(STROKE::Drawing &entry, const STROKE::Cell &cell) {
  for (const STROKE::Cell &crossed : entry.ledger)
    if (crossed == cell) return;
  entry.ledger.push_back(cell);
  entry.laid.push_back(cell);
}

auto drawn(GUI::Handle document, const STROKE::Drawing &entry) -> Whole {
  Whole deck = GUI::PARENTLESS;
  if (entry.mode.board.empty() || entry.mode.name.empty())
    return GUI::PARENTLESS;
  if (GUI::find(document, entry.mode.board.c_str(), deck) != 0)
    return GUI::PARENTLESS;
  return deck;
}

void claim(GUI::Handle document, Whole deck) {
  auto &pointer = GUI::STATE::documents[document].pointer;
  if (pointer.banding == deck) pointer.banding = GUI::PARENTLESS;
  if (pointer.cutting == deck) pointer.cutting = GUI::PARENTLESS;
}

void latch(GUI::Handle document, STROKE::Drawing &entry, Whole deck) {
  const STRING::Hot board = entry.mode.board.c_str();
  const GUI::NGA::Ask press = GUI::NGA::GET::pressed(document, board);
  const GUI::NGA::Ask stroke = GUI::NGA::GET::stroked(document, board);
  const GUI::NGA::Ask lift = GUI::NGA::GET::lifted(document, board);
  if (!press.board.empty()) STROKE::open(document, entry, press, deck);
  if (entry.gesture.standing && !stroke.board.empty())
    STROKE::walk(entry, stroke.x, stroke.y);
  if (!entry.gesture.standing || lift.board.empty()) return;
  STROKE::walk(entry, lift.x, lift.y);
  entry.gesture.standing = false;
  entry.gesture.closed = true;
}

}  // namespace

void STROKE::open(
  GUI::Handle document, Drawing &entry, const GUI::NGA::Ask &press,
  Whole deck) {
  ::claim(document, deck);
  if (entry.mode.over == Mode::GROUND && !press.target.empty()) return;
  const auto &chord = GUI::STATE::documents[document].pointer.chord;
  entry.gesture = {};
  entry.gesture.opened = true;
  entry.gesture.standing = true;
  entry.gesture.control = chord.control;
  entry.gesture.shift = chord.shift;
  entry.gesture.alt = chord.alt;
  entry.gesture.from = ::marked(entry.mode.cell, press.x, press.y);
  entry.gesture.to = entry.gesture.from;
  entry.last = entry.gesture.from.cell;
  entry.ledger.clear();
  ::lay(entry, entry.last);
}

void STROKE::walk(Drawing &entry, Float x, Float y) {
  const Mark to = ::marked(entry.mode.cell, x, y);
  entry.gesture.to = to;
  if (to.cell == entry.last) return;
  const Integer east = to.cell.across - entry.last.across;
  const Integer fall = to.cell.down - entry.last.down;
  const Integer steps = std::max({east, -east, fall, -fall});
  for (Integer step = 1; step <= steps; step += 1)
    ::lay(
      entry, {entry.last.across + east * step / steps,
              entry.last.down + fall * step / steps});
  entry.last = to.cell;
  if (to.cell != entry.gesture.from.cell) entry.gesture.dragged = true;
}

void STROKE::tend() {
  for (auto &face : drawings()) {
    const Handle document = face.first;
    if (!GUI::live(document)) continue;
    Drawing &entry = drawing(document);
    entry.laid.clear();
    if (entry.gesture.closed) entry.gesture = {};
    entry.gesture.opened = false;
    const Whole deck = ::drawn(document, entry);
    if (deck == GUI::PARENTLESS) {
      entry.gesture = {};
      continue;
    }
    ::latch(document, entry, deck);
  }
}
