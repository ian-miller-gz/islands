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

static auto shape(
  GUI::Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status {
  Float number = 0.0f;
  if (GUI::PARSE::read(value, number) != 0) return 1;
  if (key == "x" || key == "y") {
    auto position = GUI::GET::position(document, id);
    (key == "x" ? position.x : position.y) = number;
    return GUI::set(document, id, position);
  }
  auto extent = GUI::GET::extent(document, id);
  (key == "w" ? extent.w : extent.h) = number;
  return GUI::set(document, id, extent);
}

static auto gauge(
  GUI::Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status {
  Float number = 0.0f;
  if (GUI::PARSE::read(value, number) != 0) return 1;
  if (key == "fraction") return GUI::set(document, id, GUI::Fraction{number});
  if (key == "z") return GUI::set(document, id, GUI::Depth{number});
  if (key == "border") return GUI::set(document, id, GUI::Border{number});
  if (key == "size") return GUI::set(document, id, GUI::Size{number});
  return GUI::set(document, id, GUI::Pad{number});
}

static auto count(
  GUI::Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status {
  Whole number = 0;
  if (GUI::PARSE::read(value, number) != 0) return 1;
  if (key == "rows") return GUI::set(document, id, GUI::Rows{number});
  if (key == "cursor") return GUI::set(document, id, GUI::Cursor{number});
  return GUI::set(document, id, GUI::Scroll{number});
}

static auto dialled(
  GUI::Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status {
  if (key == "steps") {
    auto dial = GUI::GET::dial(document, id);
    if (GUI::PARSE::read(value, dial.steps) != 0) return 1;
    return GUI::set(document, id, dial);
  }
  if (key == "graphic") {
    auto dial = GUI::GET::dial(document, id);
    if (GUI::PARSE::read(value, dial.graphic) != 0) return 1;
    return GUI::set(document, id, dial);
  }
  Float number = 0.0f;
  if (GUI::PARSE::read(value, number) != 0) return 1;
  if (key == "value") return GUI::set(document, id, GUI::Value{number});
  auto dial = GUI::GET::dial(document, id);
  (key == "least"  ? dial.least
   : key == "most" ? dial.most
                   : dial.resting) = number;
  return GUI::set(document, id, dial);
}

auto GUI::PARSE::apply(
  Handle document, STRING::Hot id, const String &key,
  const String &value) -> Status {
  if (
    key == "value" || key == "least" || key == "most" || key == "resting" ||
    key == "steps" || key == "graphic")
    return ::dialled(document, id, key, value);
  if (key == "x" || key == "y" || key == "w" || key == "h")
    return ::shape(document, id, key, value);
  if (
    key == "fraction" || key == "z" || key == "border" || key == "pad" ||
    key == "size")
    return ::gauge(document, id, key, value);
  if (key == "rows" || key == "cursor" || key == "first")
    return ::count(document, id, key, value);
  if (key == "pitch") {
    Float number = 0.0f;
    return read(value, number) != 0 ? 1 : set(document, id, Pitch{number});
  }
  if (key == "mark") return set(document, id, Mark{value.c_str()});
  if (key == "plain") return set(document, id, Plain{value.c_str()});
  if (key == "pattern") return set(document, id, Pattern{value.c_str()});
  if (key == "text") return set(document, id, Text{value});
  if (key == "wrap") {
    Wrap wrap;
    return read(value, wrap.columns) != 0 ? 1 : set(document, id, wrap);
  }
  if (key == "visible") {
    Visibility visibility;
    return read(value, visibility.visible) != 0 ? 1
                                                : set(document, id, visibility);
  }
  if (key == "open") {
    Opening opening;
    return read(value, opening.open) != 0 ? 1 : set(document, id, opening);
  }
  if (key == "clip") {
    Clipping clipping;
    return read(value, clipping.clip) != 0 ? 1 : set(document, id, clipping);
  }
  if (key == "axis") {
    Axis axis;
    return read(value, axis) != 0 ? 1 : set(document, id, axis);
  }
  if (key == "carry") {
    Carry carry;
    return read(value, carry) != 0 ? 1 : set(document, id, carry);
  }
  if (key == "quick") {
    Quick quick;
    return read(value, quick.quick) != 0 ? 1 : set(document, id, quick);
  }
  if (key == "scroll") {
    Scrolling scrolling;
    return read(value, scrolling.scroll) != 0 ? 1
                                              : set(document, id, scrolling);
  }
  if (seated(key)) return seat(document, id, key, value);
  if (stroked(key)) return stroke(document, id, key, value);
  if (boarded(key)) return board(document, id, key, value);
  return adorn(document, id, key, value);
}
