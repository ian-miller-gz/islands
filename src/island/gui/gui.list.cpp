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
#include <island/gui/events.internal.hpp>

static auto inside(Whole value, Whole rows) -> Whole {
  const Whole last = rows > 0 ? rows - 1 : 0;
  return value < last ? value : last;
}

auto GUI::set(Handle document, STRING::Hot id, Rows rows) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  const auto cursor = ::inside(properties.cursor, rows.count);
  const auto first = ::inside(properties.first, rows.count);
  if (
    properties.rows == rows.count && properties.cursor == cursor &&
    properties.first == first)
    return 0;
  properties.rows = rows.count;
  properties.cursor = cursor;
  properties.first = first;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Cursor cursor) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  const auto row = ::inside(cursor.row, properties.rows);
  const auto window = fits(document, node);
  auto first = properties.first;
  if (window > 0) {
    if (row < first) first = row;
    if (row >= first + window) first = row - window + 1;
  }
  if (properties.cursor == row && properties.first == first) return 0;
  properties.cursor = row;
  properties.first = first;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Scroll scroll) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  const auto row = ::inside(scroll.row, properties.rows);
  if (properties.first == row) return 0;
  properties.first = row;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Pitch pitch) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.pitch == pitch.value) return 0;
  properties.pitch = pitch.value;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Mark mark) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.mark == mark.name) return 0;
  properties.mark = mark.name;
  return GUI::mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Plain plain) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.plain == plain.name) return 0;
  properties.plain = plain.name;
  return mark(document, node);
}

auto GUI::set(Handle document, STRING::Hot id, Quick quick) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  properties.quick = quick.quick;
  return 0;
}

auto GUI::set(Handle document, STRING::Hot id, Scrolling scrolling) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  auto &properties = STATE::documents[document].nodes[node].properties;
  if (properties.scroll == scrolling.scroll) return 0;
  properties.scroll = scrolling.scroll;
  return mark(document, node);
}

auto GUI::GET::rows(Handle document, STRING::Hot id) -> Whole {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0;
  return STATE::documents[document].nodes[node].properties.rows;
}

auto GUI::GET::cursor(Handle document, STRING::Hot id) -> Whole {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0;
  return STATE::documents[document].nodes[node].properties.cursor;
}

auto GUI::GET::first(Handle document, STRING::Hot id) -> Whole {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0;
  return STATE::documents[document].nodes[node].properties.first;
}

auto GUI::GET::pitch(Handle document, STRING::Hot id) -> Float {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0.0f;
  return STATE::documents[document].nodes[node].properties.pitch;
}

auto GUI::GET::quick(Handle document, STRING::Hot id) -> Flag {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return false;
  return STATE::documents[document].nodes[node].properties.quick;
}

auto GUI::GET::scrolling(Handle document, STRING::Hot id) -> Flag {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return false;
  return STATE::documents[document].nodes[node].properties.scroll;
}

auto GUI::GET::window(Handle document, STRING::Hot id) -> Whole {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 0;
  return fits(document, node);
}
