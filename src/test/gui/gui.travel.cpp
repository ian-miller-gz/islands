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
#include <test/gui/gui.internal.hpp>

namespace {

Status pull(STRING::Hot id, STRING::Hot name, Float x, Float y) {
  Status status = check("stage", GUI::stage(deck) == 0);
  status |= check("drag feeds", GUI::drag(deck, id, x, y, 1) == 0);
  step(name);
  return status;
}

Status divided() {
  using namespace GUI;
  Status status = ::pull("split", "carry", 0.0f, 30.0f);
  status |= check("carried down", GET::position(deck, "split").y == 170.0f);
  status |= check("held across", GET::position(deck, "split").x == 140.0f);
  status |= check("carry moved", GET::moved(deck, "split"));
  status |= check("moved consumed", !GET::moved(deck, "split"));
  status |= ::pull("split", "sideways", 15.0f, 0.0f);
  status |=
    check("refused axis stands", GET::position(deck, "split").y == 170.0f);
  status |= check("no travel, no move", !GET::moved(deck, "split"));
  status |= check("no travel is a click", GET::clicked(deck, "split"));
  status |= ::pull("split", "rim", 0.0f, 64.0f);
  return status |
         check("held by the parent", GET::position(deck, "split").y == 230.0f);
}

Status slid() {
  using namespace GUI;
  Status status = ::pull("rail", "slide", 40.0f, 0.0f);
  status |=
    check("across grip carries", GET::position(deck, "rail").x == 80.0f);
  status |=
    check("across grip holds down", GET::position(deck, "rail").y == 190.0f);
  return status | check("across carry moved", GET::moved(deck, "rail"));
}

Status roving() {
  using namespace GUI;
  Status status = ::pull("rover", "rove", 30.0f, 20.0f);
  status |= check("roved across", GET::position(deck, "rover").x == 270.0f);
  status |= check("roved down", GET::position(deck, "rover").y == 160.0f);
  status |= check("rove moved", GET::moved(deck, "rover"));
  status |= ::pull("rover", "lean", 2.0f, 20.0f);
  status |=
    check("lesser pull rides", GET::position(deck, "rover").x == 272.0f);
  status |=
    check("greater pull arms", GET::position(deck, "rover").y == 180.0f);
  status |= ::pull("rover", "corner", 30.0f, 45.0f);
  status |=
    check("held at the east rim", GET::position(deck, "rover").x == 300.0f);
  return status |
         check("held at the foot", GET::position(deck, "rover").y == 220.0f);
}

}  // namespace

Status travel() {
  using namespace GUI;
  Status status =
    check("grip states its axis", !GET::axis(deck, "split").across);
  status |= check("across grip states its own", GET::axis(deck, "rail").across);
  status |= check("free grip states the grant", GET::axis(deck, "rover").free);
  status |= check("a free grip is across too", GET::axis(deck, "rover").across);
  status |= ::divided();
  status |= ::slid();
  return status | ::roving();
}
