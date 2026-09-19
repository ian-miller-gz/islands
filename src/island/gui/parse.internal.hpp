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
#include <island/gui/gui.hpp>

namespace GUI::PARSE {

constexpr Whole INDENT = 2;

inline const String STYLE = "style=";

struct Cursor {
  Handle document = NONE;
  Vector<String> stack;
};

auto parse(STRING::Hot source, const Vector<String> &lines) -> Handle;

auto interpret(Cursor &cursor, const String &text, const String &site)
  -> Status;

auto define(
  Cursor &cursor, const Vector<String> &fields, Whole level,
  const String &site) -> Status;

auto include(
  Cursor &cursor, const Vector<String> &fields, Whole level,
  const String &site) -> Status;

auto link(
  Cursor &cursor, const Vector<String> &fields, Whole level,
  const String &site) -> Status;

auto dress(Cursor &cursor, const Vector<String> &fields, const String &site)
  -> Status;

auto apply(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status;
auto adorn(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status;

auto apply(Properties &properties, const String &key, const String &value)
  -> Status;
auto adorn(Properties &properties, const String &key, const String &value)
  -> Status;

auto board(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status;
auto board(Properties &properties, const String &key, const String &value)
  -> Status;

auto boarded(const String &key) -> Flag;

auto stroke(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status;
auto stroke(Properties &properties, const String &key, const String &value)
  -> Status;

auto stroked(const String &key) -> Flag;

auto seat(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status;
auto seat(Properties &properties, const String &key, const String &value)
  -> Status;

auto seated(const String &key) -> Flag;

auto strip(const String &line) -> String;

auto split(const String &line, Vector<String> &fields) -> Status;

auto measure(const String &line, Whole &level) -> Status;

auto read(const String &value, Float &out) -> Status;
auto read(const String &value, Whole &out) -> Status;
auto read(const String &value, Flag &out) -> Status;
auto read(const String &value, GFX::Color &out) -> Status;
auto read(const String &value, Anchor &out) -> Status;
auto read(const String &value, Side &out) -> Status;
auto read(const String &value, Stretch &out) -> Status;
auto read(const String &value, Axis &out) -> Status;
auto read(const String &value, Carry &out) -> Status;
auto read(const String &value, Shape &out) -> Status;
auto read(const String &value, GFX::SPRITES::Frame &out) -> Status;

auto refuse(const String &message) -> Status;

}  // namespace GUI::PARSE
