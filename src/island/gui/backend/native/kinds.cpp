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
#include <island/gui/backend/native/kinds.internal.hpp>
#include <iterator>

static constexpr auto shade(Whole red, Whole green, Whole blue, Whole alpha)
  -> GFX::Color {
  constexpr Float SCALE = 255.0f;
  return {
    static_cast<Float>(red) / SCALE, static_cast<Float>(green) / SCALE,
    static_cast<Float>(blue) / SCALE, static_cast<Float>(alpha) / SCALE};
}

static constexpr STRING::Hot WORDS[] = {
  "",       "panel", "label",  "button", "image", "bar",
  "detail", "row",   "column", "list",   "item",  "field",
  "dial",   "grip",  "graph",  "node",   "stroke"};

static const GUI::KINDS::Entry TABLE[] = {
  {WORDS[GUI::KINDS::INERT],
   {},
   0,
   {0.0f, 0.0f},
   false,
   false,
   false,
   false,
   nullptr},
  {WORDS[GUI::KINDS::PANEL],
   {.color = shade(0x18, 0x20, 0x28, 0xe0),
    .edge = shade(0x88, 0x98, 0xa8, 0xff)},
   5,
   {0.0f, 0.0f},
   false,
   false,
   false,
   false,
   GUI::KINDS::panel},
  {WORDS[GUI::KINDS::LABEL],
   {},
   0,
   {0.0f, 0.0f},
   false,
   false,
   false,
   false,
   nullptr},
  {WORDS[GUI::KINDS::BUTTON],
   {.color = shade(0x28, 0x30, 0x38, 0xf0),
    .edge = shade(0x88, 0x98, 0xa8, 0xff),
    .hover = shade(0x38, 0x42, 0x4e, 0xf0),
    .press = shade(0x10, 0x14, 0x18, 0xf0)},
   5,
   {0.0f, 0.0f},
   true,
   false,
   true,
   false,
   GUI::KINDS::button},
  {WORDS[GUI::KINDS::IMAGE],
   {},
   1,
   {0.0f, 0.0f},
   false,
   true,
   false,
   false,
   GUI::KINDS::image},
  {WORDS[GUI::KINDS::BAR],
   {.color = shade(0xc8, 0xd0, 0xdc, 0xff),
    .edge = shade(0x24, 0x2c, 0x34, 0xff)},
   2,
   {0.0f, 0.0f},
   false,
   false,
   false,
   false,
   GUI::KINDS::bar},
  {WORDS[GUI::KINDS::DETAIL],
   {.color = shade(0x18, 0x20, 0x28, 0xe0),
    .edge = shade(0x88, 0x98, 0xa8, 0xff),
    .hover = shade(0x24, 0x2e, 0x38, 0xf0),
    .press = shade(0x10, 0x14, 0x18, 0xf0),
    .pad = 6.0f},
   5,
   {0.0f, 0.0f},
   false,
   false,
   true,
   true,
   GUI::KINDS::button},
  {WORDS[GUI::KINDS::ROW],
   {.pad = 4.0f},
   0,
   {1.0f, 0.0f},
   false,
   false,
   false,
   false,
   nullptr},
  {WORDS[GUI::KINDS::COLUMN],
   {.pad = 4.0f},
   0,
   {0.0f, 1.0f},
   false,
   false,
   false,
   false,
   nullptr},
  {WORDS[GUI::KINDS::LIST],
   {.color = shade(0x18, 0x20, 0x28, 0xe0),
    .edge = shade(0x88, 0x98, 0xa8, 0xff),
    .clip = true,
    .pitch = 18.0f},
   5,
   {0.0f, 1.0f},
   false,
   false,
   false,
   false,
   GUI::KINDS::panel},
  {WORDS[GUI::KINDS::ITEM],
   {.color = shade(0x00, 0x00, 0x00, 0x00),
    .edge = shade(0xc8, 0xd0, 0xdc, 0xff),
    .hover = shade(0x38, 0x42, 0x4e, 0x80),
    .press = shade(0x10, 0x14, 0x18, 0x80),
    .pad = 4.0f},
   11,
   {0.0f, 0.0f},
   false,
   false,
   true,
   false,
   GUI::KINDS::item},
  {WORDS[GUI::KINDS::FIELD],
   {.color = shade(0x10, 0x14, 0x18, 0xf0),
    .edge = shade(0xc8, 0xd0, 0xdc, 0xff),
    .hover = shade(0x2b, 0x6c, 0xb0, 0x80),
    .press = shade(0x10, 0x14, 0x18, 0xf0),
    .pad = 4.0f},
   7,
   {0.0f, 0.0f},
   false,
   false,
   true,
   false,
   GUI::KINDS::field},
  {WORDS[GUI::KINDS::DIAL],
   {.color = shade(0x10, 0x14, 0x18, 0xf0),
    .edge = shade(0xc8, 0xd0, 0xdc, 0xff),
    .hover = shade(0x2b, 0x6c, 0xb0, 0x80),
    .press = shade(0x2b, 0x6c, 0xb0, 0xc0),
    .pad = 4.0f},
   8,
   {0.0f, 0.0f},
   false,
   false,
   true,
   false,
   GUI::KINDS::dial},
  {WORDS[GUI::KINDS::GRIP],
   {.color = shade(0x28, 0x30, 0x38, 0xf0),
    .edge = shade(0x88, 0x98, 0xa8, 0xff),
    .hover = shade(0x38, 0x42, 0x4e, 0xf0),
    .press = shade(0x44, 0x50, 0x5e, 0xf0)},
   5,
   {0.0f, 0.0f},
   false,
   false,
   true,
   false,
   GUI::KINDS::button},
  {WORDS[GUI::KINDS::GRAPH],
   {.color = shade(0x10, 0x14, 0x18, 0xf0),
    .edge = shade(0x50, 0x5c, 0x68, 0xff),
    .hover = shade(0x2b, 0x6c, 0xb0, 0x40),
    .clip = true},
   5,
   {0.0f, 0.0f},
   false,
   false,
   false,
   false,
   GUI::KINDS::panel},
  {WORDS[GUI::KINDS::NODE],
   {.color = shade(0x28, 0x30, 0x38, 0xf0),
    .edge = shade(0x88, 0x98, 0xa8, 0xff),
    .hover = shade(0x38, 0x42, 0x4e, 0xf0),
    .press = shade(0x44, 0x50, 0x5e, 0xf0),
    .pad = 4.0f},
   5,
   {0.0f, 0.0f},
   false,
   false,
   true,
   false,
   GUI::KINDS::button},
  {WORDS[GUI::KINDS::STROKE],
   {.color = shade(0xc8, 0xd0, 0xdc, 0xff), .border = 2.0f},
   GUI::KINDS::SEGMENTS,
   {0.0f, 0.0f},
   false,
   false,
   false,
   false,
   GUI::KINDS::stroke},
};

static_assert(std::size(TABLE) == std::size(WORDS));
static_assert(std::size(TABLE) == GUI::KINDS::STROKE + 1);

auto GUI::KINDS::row(const String &kind) -> Whole {
  for (Whole at = INERT + 1; at < std::size(WORDS); at += 1)
    if (kind == WORDS[at]) return at;
  return INERT;
}

auto GUI::KINDS::entry(const String &kind) -> const Entry & {
  return TABLE[row(kind)];
}

auto GUI::KINDS::entry(const Node &node) -> const Entry & {
  return TABLE[node.row < std::size(TABLE) ? node.row : INERT];
}

auto GUI::KINDS::known(const String &kind) -> Flag {
  return row(kind) != INERT;
}

auto GUI::BACKEND::defaults(STRING::Hot kind) -> Properties {
  return KINDS::entry(kind).defaults;
}

auto GUI::BACKEND::known(STRING::Hot kind) -> Flag {
  return KINDS::known(kind);
}

auto GUI::BACKEND::row(STRING::Hot kind) -> Whole { return KINDS::row(kind); }
