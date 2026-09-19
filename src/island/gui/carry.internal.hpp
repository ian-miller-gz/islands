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
#pragma once
#include <island/gui/carry.hpp>

namespace GUI::SAC::CARRY {

struct Mate {
  STRING::Cold id;
  Position off;
};

struct Held {
  Whole generation = 0;
  Watch watched;
  STRING::Cold leader;
  Vector<Mate> mates;
  Vector<Drop> dropped;
};

auto holding(Handle document) -> Held &;

auto holds() -> Map<Handle, Held> &;

auto refused() -> Flag;

void tend();

void grab(Handle document, Held &entry, Whole deck);
void tow(Handle document, const Held &entry);
void rest(Handle document, const Held &entry);

}  // namespace GUI::SAC::CARRY
