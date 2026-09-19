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
#include <island/gui/dialog.internal.hpp>

namespace {
using namespace GUI;

constexpr STRING::Hot SOURCE = "islands.dialog";

constexpr STRING::Hot TITLES[] = {"Open File", "Save File"};
constexpr STRING::Hot ACCEPTS[] = {"Open", "Save"};

constexpr STRING::Hot LAYOUT =
  "style=assets/islands.gui\n"
  "panel frame style=sheet anchor=center w=460 h=380\n"
  "  label title style=ink size=20 x=16 y=12\n"
  "  label where style=muted size=16 x=16 y=44\n"
  "  list listing style=box x=16 y=68 w=428 h=208 pitch=22 size=16"
  " mark=selected plain=entry\n"
  "  label prompt style=ink size=16 x=16 y=292 text=\"Path:\"\n"
  "  field field style=paper x=64 y=288 w=380 h=26 size=16\n"
  "  button cancel style=place size=16 x=272 y=332 w=80 h=30"
  " text=\"Cancel\"\n"
  "  button ok style=call size=16 x=360 y=332 w=80 h=30\n";
}  // namespace

auto GUI::DIALOG::compose(Mode mode, const String &text) -> Handle {
  const String layout = LAYOUT;
  const Handle document =
    load(SOURCE, Vector<Byte>(layout.begin(), layout.end()));
  if (document == NONE) return NONE;
  set(document, "title", Text{TITLES[mode]});
  set(document, OK, Text{ACCEPTS[mode]});
  set(document, FIELD, Text{text});
  return document;
}
