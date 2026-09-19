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
#include <island/gui/parse.internal.hpp>

auto GUI::PARSE::seated(const String &key) -> Flag {
  return key == "beside" || key == "side" || key == "air";
}

auto GUI::PARSE::seat(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status {
  Seat seat = GET::seat(document, id);
  if (key == "beside") {
    seat.beside = value.c_str();
    return set(document, id, seat);
  }
  if (key == "side")
    return read(value, seat.side) != 0 ? 1 : set(document, id, seat);
  return read(value, seat.air) != 0 ? 1 : set(document, id, seat);
}

auto GUI::PARSE::seat(
  Properties &properties, const String &key, const String &value) -> Status {
  if (key == "beside") {
    properties.beside = value.c_str();
    return 0;
  }
  if (key == "side") {
    Side side = BELOW;
    if (read(value, side) != 0) return 1;
    properties.side = static_cast<Whole>(side);
    return 0;
  }
  Float air = 0.0f;
  if (read(value, air) != 0) return 1;
  properties.air = air;
  return 0;
}
