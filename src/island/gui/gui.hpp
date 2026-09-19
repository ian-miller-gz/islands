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
#include <island/graphics/color.hpp>
#include <island/graphics/sprites/sprites.hpp>
#include <island/input.hpp>
#include <limits>

namespace GUI {

using Handle = Whole;

constexpr Handle NONE = static_cast<Handle>(-1);

constexpr GFX::SPRITES::Handle UNBOUND = static_cast<GFX::SPRITES::Handle>(-1);

constexpr GFX::SPRITES::Frame FULL = {0.0f, 0.0f, 1.0f, 1.0f};

enum State { REST, HOVER, PRESS };

struct Event {
  enum Kind {
    ENTERED,
    EXITED,
    PRESSED,
    CLICKED,
    SELECTED,
    ACTIVATED,
    EDITED,
    COMMITTED,
    DIALLED,
    MOVED,
    JOINED,
    CUT
  };
  STRING::Cold id;
  Kind kind = ENTERED;
};

enum Anchor { NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST, CENTER };

enum Side { BELOW, ABOVE, EAST, WEST };

struct Seat {
  STRING::Cold beside;
  Side side = BELOW;
  Float air = 0.0f;
  auto operator==(const Seat &) const -> Bool = default;
};

struct Room {
  Float west = 0.0f;
  Float east = 0.0f;
  Float north = 0.0f;
  Float south = 0.0f;
  auto operator==(const Room &) const -> Bool = default;
};

struct Pool {
  Whole live = 0;
  Whole dead = 0;
  auto operator==(const Pool &) const -> Bool = default;
};

struct Position {
  Float x = 0.0f;
  Float y = 0.0f;
  auto operator==(const Position &) const -> Bool = default;
};

struct Extent {
  Float w = 0.0f;
  Float h = 0.0f;
  auto operator==(const Extent &) const -> Bool = default;
};

struct Walls {
  static constexpr Float NONE = std::numeric_limits<Float>::infinity();
  Float west = NONE;
  Float east = NONE;
  Float north = NONE;
  Float south = NONE;
  auto operator==(const Walls &) const -> Bool = default;
};

namespace EDGE {
constexpr Whole LEFT = 1;
constexpr Whole RIGHT = 2;
constexpr Whole TOP = 4;
constexpr Whole BOTTOM = 8;
constexpr Whole FILL = LEFT | RIGHT | TOP | BOTTOM;
}  // namespace EDGE

struct Stretch {
  Whole edges = 0;
};

struct Text {
  STRING::Cold value;
};

struct Fraction {
  Float value = 0.0f;
};

struct Value {
  Float value = 0.0f;
};

struct Dial {
  Float least = 0.0f;
  Float most = 0.0f;
  Float resting = 0.0f;
  Whole steps = 0;
  Flag graphic = false;
};

struct Visibility {
  Flag visible = true;
};

struct Depth {
  Float value = 0.0f;
};

struct Border {
  Float width = 0.0f;
};

struct Edge {
  GFX::Color color = GFX::COLORS::WHITE;
};

struct Hover {
  GFX::Color color = GFX::COLORS::WHITE;
};

struct Press {
  GFX::Color color = GFX::COLORS::WHITE;
};

struct Glow {
  GFX::Color color = GFX::COLORS::CLEAR;
};

struct Wash {
  GFX::Color color = GFX::COLORS::CLEAR;
};

struct Pad {
  Float value = 0.0f;
};

struct Size {
  Float value = 0.0f;
};

struct Wrap {
  Whole columns = 0;
};

struct Opening {
  Flag open = false;
};

struct Clipping {
  Flag clip = false;
};

struct Rows {
  Whole count = 0;
};

struct Cursor {
  Whole row = 0;
};

struct Scroll {
  Whole row = 0;
};

struct Pitch {
  Float value = 0.0f;
};

struct Mark {
  STRING::Hot name = "";
};

struct Plain {
  STRING::Hot name = "";
};

struct Pattern {
  STRING::Hot name = "";
};

struct Quick {
  Flag quick = false;
};

struct Scrolling {
  Flag scroll = true;
};

struct Axis {
  Flag across = false;
  Flag free = false;
};

struct Carry {
  Flag pointer = false;
};

struct Fold {
  enum Stand { NONE, CLOSED, OPEN };
  Stand stand = NONE;
};

struct Caret {
  Whole at = 0;
  Whole from = 0;
};

struct Ends {
  Position to;
};

struct Shape {
  enum Run { LINE, RISE, FALL, EASE, HOLD };
  Run run = LINE;
};

struct Bend {
  Float value = 0.0f;
};

struct Image {
  STRING::Cold atlas;
  Whole column = 0;
  Whole row = 0;
};

struct Style {
  STRING::Hot name = "";
};

struct Properties {
  Position position;
  Extent extent;
  Anchor anchor = NORTHWEST;
  STRING::Cold beside;
  Whole side = 0;
  Float air = 0.0f;
  GFX::Color color = GFX::COLORS::WHITE;
  STRING::Cold text;
  GFX::SPRITES::Frame frame = FULL;
  GFX::SPRITES::Handle atlas = UNBOUND;
  STRING::Cold atlas_name;
  Float fraction = 0.0f;
  Flag visible = true;
  Float z = 0.0f;
  Float border = 0.0f;
  GFX::Color edge = GFX::COLORS::WHITE;
  GFX::Color hover = GFX::COLORS::WHITE;
  GFX::Color press = GFX::COLORS::WHITE;
  GFX::Color glow = GFX::COLORS::CLEAR;
  Float pad = 0.0f;
  Whole wrap = 0;
  Float size = 0.0f;
  Whole pins = 0;
  Flag open = false;
  Flag clip = false;
  Whole rows = 0;
  Whole cursor = 0;
  Whole first = 0;
  Float pitch = 0.0f;
  STRING::Cold mark;
  STRING::Cold plain;
  STRING::Cold pattern;
  Flag quick = false;
  Flag scroll = true;
  Flag across = false;
  Flag free = false;
  Flag carried = false;
  Whole fold = 0;
  Whole caret = 0;
  Whole selection = 0;
  Flag seated = false;
  Float value = 0.0f;
  Float least = 0.0f;
  Float most = 0.0f;
  Float resting = 0.0f;
  Whole steps = 0;
  Flag graphic = false;
  Whole port = 0;
  STRING::Cold kind;
  Flag mute = false;
  Whole pinned = 0;
  Flag selected = false;
  GFX::Color wash = {0.169f, 0.424f, 0.690f, 0.25f};
  Position pan;
  Extent zoom = {1.0f, 1.0f};
  Walls walls;
  Flag banded = false;
  Position to;
  Whole shape = 0;
  Float bend = 0.0f;
};

auto load(STRING::Hot path) -> Handle;

auto load(STRING::Hot name, const Vector<Byte> &document) -> Handle;

auto mount(STRING::Hot assets, STRING::Hot layout) -> Handle;

void remove(Handle document);

enum Theme : Whole { LIGHT, DARK };

void theme(Theme shade);

void bind(STRING::Hot name, GFX::SPRITES::Handle atlas);

auto set(Handle document, STRING::Hot id, Position position) -> Status;

auto set(Handle document, STRING::Hot id, Extent extent) -> Status;

auto set(Handle document, STRING::Hot id, GFX::Color color) -> Status;

auto set(Handle document, STRING::Hot id, Text text) -> Status;

auto set(Handle document, STRING::Hot id, Fraction fraction) -> Status;

auto set(Handle document, STRING::Hot id, Value value) -> Status;

auto set(Handle document, STRING::Hot id, Dial dial) -> Status;

auto set(Handle document, STRING::Hot id, Visibility visibility) -> Status;

auto set(Handle document, STRING::Hot id, Anchor anchor) -> Status;

auto set(Handle document, STRING::Hot id, Seat seat) -> Status;

auto set(Handle document, STRING::Hot id, Stretch stretch) -> Status;

auto set(Handle document, STRING::Hot id, GFX::SPRITES::Frame frame) -> Status;

auto set(Handle document, STRING::Hot id, Depth depth) -> Status;

auto set(Handle document, STRING::Hot id, Border border) -> Status;

auto set(Handle document, STRING::Hot id, Edge edge) -> Status;

auto set(Handle document, STRING::Hot id, Hover hover) -> Status;

auto set(Handle document, STRING::Hot id, Press press) -> Status;

auto set(Handle document, STRING::Hot id, Glow glow) -> Status;

auto set(Handle document, STRING::Hot id, Wash wash) -> Status;

auto set(Handle document, STRING::Hot id, Pad pad) -> Status;

auto set(Handle document, STRING::Hot id, Size size) -> Status;

auto set(Handle document, STRING::Hot id, Wrap wrap) -> Status;

auto set(Handle document, STRING::Hot id, Opening opening) -> Status;

auto set(Handle document, STRING::Hot id, Clipping clipping) -> Status;

auto set(Handle document, STRING::Hot id, Rows rows) -> Status;

auto set(Handle document, STRING::Hot id, Cursor cursor) -> Status;

auto set(Handle document, STRING::Hot id, Scroll scroll) -> Status;

auto set(Handle document, STRING::Hot id, Pitch pitch) -> Status;

auto set(Handle document, STRING::Hot id, Mark mark) -> Status;

auto set(Handle document, STRING::Hot id, Plain plain) -> Status;

auto set(Handle document, STRING::Hot id, Pattern pattern) -> Status;

auto set(Handle document, STRING::Hot id, Quick quick) -> Status;

auto set(Handle document, STRING::Hot id, Scrolling scrolling) -> Status;

auto set(Handle document, STRING::Hot id, Axis axis) -> Status;

auto set(Handle document, STRING::Hot id, Carry carry) -> Status;

auto set(Handle document, STRING::Hot id, Fold fold) -> Status;

auto set(Handle document, STRING::Hot id, Caret caret) -> Status;

auto set(Handle document, STRING::Hot id, Ends ends) -> Status;

auto set(Handle document, STRING::Hot id, Shape shape) -> Status;

auto set(Handle document, STRING::Hot id, Bend bend) -> Status;

auto set(Handle document, STRING::Hot id, Image image) -> Status;

auto set(Handle document, STRING::Hot id, Style style) -> Status;

auto place(Handle document, GFX::Viewport viewport) -> Status;

auto point(Handle document, Float u, Float v, Flag down) -> Status;

auto stage(Handle document) -> Status;

auto click(Handle document, STRING::Hot id) -> Status;

auto doubleclick(Handle document, STRING::Hot id) -> Status;

auto drag(
  Handle document, STRING::Hot from, STRING::Hot to,
  const INPUT::Chord &chord = {}) -> Status;

auto drag(
  Handle document, STRING::Hot id, Float x, Float y, Whole holds,
  const INPUT::Chord &chord = {}) -> Status;

auto press(
  Handle document, STRING::Hot id, Float x, Float y,
  const INPUT::Chord &chord = {}) -> Status;

auto release(Handle document, Float x, Float y) -> Status;

auto travel(Handle document, Float x, Float y) -> Status;

auto ask(Handle document, STRING::Hot id, Float x, Float y) -> Status;

auto wheel(Handle document, STRING::Hot id, Float turns, Flag control)
  -> Status;

auto key(Handle document, const INPUT::KEYS::Event &event) -> Status;

auto focus(Handle document, Flag on) -> Status;

auto claim(Handle document, Flag on) -> Status;

auto edit(Handle document, STRING::Hot id) -> Status;

void poll();

void flush(Handle document, GFX::Pass &pass);

auto frame(Handle document, GFX::Color clear) -> Status;

void close();

void reset();

}  // namespace GUI

namespace GUI::NODES {

constexpr STRING::Hot ROOT = "";

auto create(
  Handle document, STRING::Hot parent, STRING::Hot kind,
  STRING::Hot id) -> Status;

auto remove(Handle document, STRING::Hot id) -> Status;

}  // namespace GUI::NODES

namespace GUI::GET {

auto document(STRING::Hot source) -> Handle;

auto document() -> Handle;

auto theme() -> Theme;

auto position(Handle document, STRING::Hot id) -> Position;

auto extent(Handle document, STRING::Hot id) -> Extent;

auto color(Handle document, STRING::Hot id) -> GFX::Color;

auto text(Handle document, STRING::Hot id) -> STRING::Cold;

auto fraction(Handle document, STRING::Hot id) -> Float;

auto value(Handle document, STRING::Hot id) -> Float;

auto dial(Handle document, STRING::Hot id) -> Dial;

auto axis(Handle document, STRING::Hot id) -> Axis;

auto carry(Handle document, STRING::Hot id) -> Carry;

auto visibility(Handle document, STRING::Hot id) -> Flag;

auto anchor(Handle document, STRING::Hot id) -> Anchor;

auto seat(Handle document, STRING::Hot id) -> Seat;

auto room(Handle document, STRING::Hot id) -> Room;

auto stretch(Handle document, STRING::Hot id) -> Whole;

auto measured(Handle document, STRING::Hot id) -> Extent;

auto origin(Handle document, STRING::Hot id) -> Position;

auto ends(Handle document, STRING::Hot id) -> Ends;

auto shape(Handle document, STRING::Hot id) -> Shape;

auto bend(Handle document, STRING::Hot id) -> Float;

auto frame(Handle document, STRING::Hot id) -> GFX::SPRITES::Frame;

auto depth(Handle document, STRING::Hot id) -> Float;

auto border(Handle document, STRING::Hot id) -> Float;

auto edge(Handle document, STRING::Hot id) -> GFX::Color;

auto hover(Handle document, STRING::Hot id) -> GFX::Color;

auto press(Handle document, STRING::Hot id) -> GFX::Color;

auto glow(Handle document, STRING::Hot id) -> GFX::Color;

auto wash(Handle document, STRING::Hot id) -> GFX::Color;

auto pad(Handle document, STRING::Hot id) -> Float;

auto size(Handle document, STRING::Hot id) -> Float;

auto wrap(Handle document, STRING::Hot id) -> Whole;

auto opening(Handle document, STRING::Hot id) -> Flag;

auto clipping(Handle document, STRING::Hot id) -> Flag;

auto rows(Handle document, STRING::Hot id) -> Whole;

auto cursor(Handle document, STRING::Hot id) -> Whole;

auto first(Handle document, STRING::Hot id) -> Whole;

auto pitch(Handle document, STRING::Hot id) -> Float;

auto quick(Handle document, STRING::Hot id) -> Flag;

auto scrolling(Handle document, STRING::Hot id) -> Flag;

auto window(Handle document, STRING::Hot id) -> Whole;

auto fold(Handle document, STRING::Hot id) -> Fold;

auto caret(Handle document, STRING::Hot id) -> Caret;

auto editing(Handle document) -> STRING::Cold;

auto style(Handle document, STRING::Hot name) -> Properties;

auto dirty(Handle document) -> Flag;

auto dirty(Handle document, STRING::Hot id) -> Flag;

auto restructured(Handle document) -> Flag;

auto generation(Handle document) -> Whole;

auto hover(Handle document) -> STRING::Cold;

auto pressed(Handle document) -> Flag;

auto clicked(Handle document, STRING::Hot id) -> Flag;

auto selected(Handle document, STRING::Hot id) -> Flag;

auto activated(Handle document, STRING::Hot id) -> Flag;

auto edited(Handle document, STRING::Hot id) -> Flag;

auto committed(Handle document, STRING::Hot id) -> Flag;

auto dialled(Handle document, STRING::Hot id) -> Flag;

auto moved(Handle document, STRING::Hot id) -> Flag;

auto keyed(Handle document) -> INPUT::KEYS::Event;

auto events(Handle document) -> const Vector<Event> &;

auto polls() -> Whole;

auto claimed() -> Handle;

auto under(Float x, Float y, Handle behind) -> Flag;

auto writes() -> Whole;

auto nodes(Handle document) -> Pool;

}  // namespace GUI::GET
