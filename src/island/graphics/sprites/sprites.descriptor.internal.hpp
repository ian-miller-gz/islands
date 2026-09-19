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
#include <island/graphics/sprites/sprites.internal.hpp>

namespace GFX::SPRITES::DESCRIPTOR {

struct Cut {
  ATLASES::Point origin;
  ATLASES::Placement placement;
  Flag sourced = false;
};

struct Cursor {
  String image;
  ATLASES::Cell cell;
  Vector<Cut> cuts;
  Vector<String> names;
};

auto interpret(Cursor &cursor, const String &line, const String &site)
  -> Status;

auto refuse(const String &site, const String &reason) -> Status;

constexpr Status UNKNOWN = 1;
constexpr Status MALFORMED = 2;

auto number(const String &text, Whole &answer) -> Status;

auto apply(const String &key, const String &value, Cut &cut) -> Status;

}  // namespace GFX::SPRITES::DESCRIPTOR
