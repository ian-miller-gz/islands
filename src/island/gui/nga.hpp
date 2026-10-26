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

#include <common.hpp>
#include <island/gui/gui.hpp>

namespace GUI::NGA {

struct Link {
  STRING::Cold from;
  STRING::Cold to;
  STRING::Cold label;
  STRING::Cold style;
};

struct Label {
  STRING::Hot value = "";
};

struct Ask {
  STRING::Cold board;
  STRING::Cold target;
  Float x = 0.0f;
  Float y = 0.0f;
  Flag asked = false;
};

struct Wheel {
  Float turns = 0.0f;
  Flag control = false;
  Float across = 0.0f;
  Float down = 0.0f;
};

struct Port {
  enum Side { NONE, IN, OUT };
  Side side = NONE;
  STRING::Cold kind;
  Flag mute = false;
};

struct Selected {
  Flag on = false;
};

struct Choosing {
  Flag band = false;
};

struct Pan {
  Float x = 0.0f;
  Float y = 0.0f;
};

struct Zoom {
  static constexpr Float SAME = 0.0f;
  Float value = 1.0f;
  Float down = SAME;
};

struct Pinned {
  enum Axis { NONE, ACROSS, DOWN };
  Axis axis = NONE;
};

using Bounds = Walls;

auto set(Handle document, STRING::Hot id, Port port) -> Status;

auto set(Handle document, STRING::Hot id, Selected selected) -> Status;

auto set(Handle document, STRING::Hot id, Pan pan) -> Status;

auto set(Handle document, STRING::Hot id, Zoom zoom) -> Status;

auto set(Handle document, STRING::Hot id, Pinned pinned) -> Status;

auto set(Handle document, STRING::Hot id, Choosing choosing) -> Status;

auto set(Handle document, STRING::Hot id, Bounds bounds) -> Status;

auto connect(Handle document, STRING::Hot from, STRING::Hot to) -> Status;

auto connect(Handle document, STRING::Hot from, STRING::Hot to, Label label)
  -> Status;

auto connect(Handle document, STRING::Hot from, STRING::Hot to, Style style)
  -> Status;

auto disconnect(Handle document, STRING::Hot from, STRING::Hot to) -> Status;

auto clear(Handle document, STRING::Hot id) -> Status;

}  // namespace GUI::NGA

namespace GUI::NGA::GET {

auto port(Handle document, STRING::Hot id) -> Port;

auto selected(Handle document, STRING::Hot id) -> Flag;

auto selections(Handle document, STRING::Hot id) -> Vector<STRING::Cold>;

auto pan(Handle document, STRING::Hot id) -> Pan;

auto zoom(Handle document, STRING::Hot id) -> Zoom;

auto pinned(Handle document, STRING::Hot id) -> Pinned;

auto choosing(Handle document, STRING::Hot id) -> Choosing;

auto bounds(Handle document, STRING::Hot id) -> Bounds;

auto links(Handle document, STRING::Hot id) -> Vector<Link>;

auto linked(Handle document, STRING::Hot from, STRING::Hot to) -> Flag;

auto label(Handle document, STRING::Hot from, STRING::Hot to) -> STRING::Cold;

auto ports(Handle document, STRING::Hot id) -> Vector<STRING::Cold>;

auto reaches(Handle document, STRING::Hot from, STRING::Hot to) -> Flag;

auto order(Handle document, STRING::Hot id, Vector<STRING::Cold> &boxes)
  -> Status;

auto orphans(Handle document, STRING::Hot id, Vector<STRING::Cold> &boxes)
  -> Status;

auto wiring(Handle document) -> STRING::Cold;

auto aimed(Handle document) -> STRING::Cold;

auto joined(Handle document) -> Vector<Link>;

auto asked(Handle document) -> Ask;

auto pressed(Handle document, STRING::Hot id) -> Ask;

auto stroked(Handle document, STRING::Hot id) -> Ask;

auto lifted(Handle document, STRING::Hot id) -> Ask;

auto wheel(Handle document, STRING::Hot id) -> Wheel;

auto cut(Handle document) -> Vector<Link>;

}  // namespace GUI::NGA::GET
