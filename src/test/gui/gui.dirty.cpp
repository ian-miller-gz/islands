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
#include <island/gui/gui.internal.hpp>

static Status discipline(GUI::Handle document) {
  using namespace GUI;
  Status status = check(
    "born dirty", GET::dirty(document) && GET::restructured(document) &&
                    GET::dirty(document, "menu"));
  clean(document);
  status |= check(
    "clean lowers all", !GET::dirty(document) && !GET::restructured(document) &&
                          !GET::dirty(document, "menu"));
  status |= check(
    "write marks node and document",
    set(document, "title", Fraction{1.0f}) == 0 &&
      GET::dirty(document, "title") && GET::dirty(document) &&
      !GET::dirty(document, "menu") && !GET::restructured(document));
  return status;
}

static Status removal(GUI::Handle document) {
  using namespace GUI;
  clean(document);
  Status status = check(
    "remove restructures", NODES::remove(document, "menu") == 0 &&
                             GET::restructured(document) &&
                             GET::dirty(document));
  status |= check(
    "remove retires the subtree", set(document, "title", Fraction{0.0f}) != 0 &&
                                    !GET::dirty(document, "title"));
  status |=
    check("refuse unknown remove", NODES::remove(document, "menu") != 0);
  status |= check(
    "free removed ids",
    NODES::create(document, NODES::ROOT, "panel", "menu") == 0);
  return status;
}

Status track(GUI::Handle document) {
  return discipline(document) | removal(document);
}
