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
#include <island/gui/seat.hpp>
#include <test/gui/gui.internal.hpp>

namespace {
constexpr STRING::Hot DECK = "deck";
constexpr STRING::Hot LAYOUT =
  "graph deck x=0 y=0 w=200 h=100\n"
  "  node deck.0 x=0 y=0 w=40 h=20 text=\"first\"\n"
  "  node deck.1 x=0 y=20 w=40 h=20 text=\"second\"\n"
  "    panel deck.1.grip x=30 y=0 w=10 h=20\n"
  "  panel deck.band x=0 y=60 w=200 h=20\n";
}  // namespace

Status seated() {
  using namespace GUI::SAC;
  Status status = check(
    "the id is the board's stem and the row's count",
    SEAT::named(::DECK, 3) == "deck.3");
  status |= check(
    "and the first row counts from zero", SEAT::named(::DECK, 0) == "deck.0");
  status |=
    check("the row reads back out of it", SEAT::row("deck.3", ::DECK) == 3);
  status |= check(
    "a child of a seat names its seat", SEAT::row("deck.3.grip", ::DECK) == 3);
  status |= check(
    "the board's own furniture names no row",
    SEAT::row("deck.band", ::DECK) == SEAT::NONE);
  status |= check(
    "another board's widget names no row",
    SEAT::row("desk.3", ::DECK) == SEAT::NONE);
  status |= check(
    "the bare board names no row", SEAT::row("deck", ::DECK) == SEAT::NONE);
  status |= check(
    "and a token that is not a count names none",
    SEAT::row("deck.3x", ::DECK) == SEAT::NONE);
  const String layout = ::LAYOUT;
  const GUI::Handle deck =
    GUI::load("seat", Vector<Byte>(layout.begin(), layout.end()));
  status |= check("board load", deck != GUI::NONE);
  status |= check(
    "undesignated reads no selection",
    SEAT::selected(deck, ::DECK) == SEAT::NONE);
  status |= check("the name adopts", adopt({String(SEAT::NAME)}) == 0);
  status |= check(
    "nothing selected names no row",
    SEAT::selected(deck, ::DECK) == SEAT::NONE);
  status |= check(
    "select the second seat",
    GUI::NGA::set(deck, "deck.1", GUI::NGA::Selected{true}) == 0);
  status |=
    check("the selection names its row", SEAT::selected(deck, ::DECK) == 1);
  status |= check("re-adoption replaces whole", adopt({}) == 0);
  status |=
    check("the door closed again", SEAT::selected(deck, ::DECK) == SEAT::NONE);
  return status;
}
