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
#include <test/gui/gui.internal.hpp>

Status styles() {
  constexpr STRING::Hot TEXT =
    "style mark color=#9ec5e8ff\n"
    "style boxed color=#e2e8f0ff edge=#1a202cff border=2\n"
    "style broken color=nonsense\n"
    "panel sheet style=boxed x=8 y=8 w=64 h=32\n";
  const String source(TEXT);
  const auto document =
    GUI::load("styles", Vector<Byte>(source.begin(), source.end()));
  Status status = check("styles document loads", document != GUI::NONE);
  const auto worn = GUI::GET::style(document, "boxed");
  status |= check(
    "style read projects the stated set",
    worn.color == GUI::GET::color(document, "sheet") && worn.border == 2.0f &&
      worn.extent.w == 0.0f);
  status |= check(
    "style read misses to the zero block",
    GUI::GET::style(document, "ghost").border == 0.0f &&
      GUI::GET::style(document, "ghost").color == GFX::COLORS::WHITE);
  status |= check(
    "style write assigns the stated keys",
    GUI::set(document, "sheet", GUI::Style{"mark"}) == 0 &&
      GUI::GET::color(document, "sheet") ==
        GUI::GET::style(document, "mark").color &&
      GUI::GET::border(document, "sheet") == 2.0f);
  GUI::clean(document);
  status |= check(
    "standing style re-assigns clean",
    GUI::set(document, "sheet", GUI::Style{"mark"}) == 0 &&
      !GUI::GET::dirty(document));
  status |= check(
    "unknown style refuses and changes nothing",
    GUI::set(document, "sheet", GUI::Style{"ghost"}) != 0 &&
      !GUI::GET::dirty(document));
  status |= check(
    "unchecked definition refuses at assignment",
    GUI::set(document, "sheet", GUI::Style{"broken"}) != 0 &&
      GUI::GET::color(document, "sheet") ==
        GUI::GET::style(document, "mark").color);
  GUI::remove(document);
  return status;
}
