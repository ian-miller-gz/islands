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
#include <iostream>
#include <test/gui/gui.internal.hpp>

Status designated() {
  using namespace GUI;
  Status status = check("nothing adopted", !SAC::GET::adopted(DIALOG::NAME));
  DIALOG::open(DIALOG::OPEN, "");
  status |=
    check("open without designation raises nothing", !DIALOG::GET::active());
  status |= check(
    "an unknown name is refused by count",
    SAC::adopt({String(DIALOG::NAME), "nonesuch"}) == 1);
  status |= check("the known name adopted", SAC::GET::adopted(DIALOG::NAME));
  status |= check("the unknown name is not", !SAC::GET::adopted("nonesuch"));
  DIALOG::open(DIALOG::OPEN, "");
  status |= check("designated raises", DIALOG::GET::active());
  DIALOG::close();
  status |= check("re-adoption replaces whole", SAC::adopt({}) == 0);
  status |= check("the designation is gone", !SAC::GET::adopted(DIALOG::NAME));
  DIALOG::open(DIALOG::OPEN, "");
  status |= check("the door closed again", !DIALOG::GET::active());
  return status;
}
