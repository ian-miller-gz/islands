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
#include <island/gui/nga.hpp>

namespace GUI {

constexpr Whole PARENTLESS = static_cast<Whole>(-1);

struct Pointer {
  Flag placed = false;
  Flag fed = false;
  Flag staged = false;
  Flag holding = false;
  Position hold;
  Flag holdin = false;
  GFX::Viewport placement;
  Whole hovered = PARENTLESS;
  Whole pressed = PARENTLESS;
  INPUT::Chord chord;
  Flag down = false;
  Flag landed = false;
  Vector<Event> events;
  Whole armed = PARENTLESS;
  Whole struck = 0;
  Float stamp = 0.0f;
  Position spot;
  Vector<STRING::Cold> clicks;
  Vector<STRING::Cold> selections;
  Vector<STRING::Cold> activations;
  Vector<STRING::Cold> edits;
  Vector<STRING::Cold> commits;
  Vector<STRING::Cold> turns;
  Vector<STRING::Cold> moves;
  Position grab;
  Float grabbed = 0.0f;
  Position standing;
  Flag dragged = false;
  Whole wiring = PARENTLESS;
  Whole cutting = PARENTLESS;
  Whole banding = PARENTLESS;
  Position reach;
  Whole snapped = PARENTLESS;
  Vector<NGA::Link> joins;
  Vector<NGA::Link> cuts;
  Whole panning = PARENTLESS;
  Flag panned = false;
  Flag swept = false;
  Whole asking = PARENTLESS;
  Position onset;
  Position viewed;
  NGA::Ask ask;
  NGA::Wheel turned;
  Whole spun = PARENTLESS;
  NGA::Ask press;
  Whole pressing = PARENTLESS;
  NGA::Ask stroke;
  Whole stroking = PARENTLESS;
  NGA::Ask lift;
  Whole lifting = PARENTLESS;
};

}  // namespace GUI
