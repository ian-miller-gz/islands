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
#include <island/gui/translate.internal.hpp>

namespace {

struct Word {
  STRING::Hot word;
  GUI::Dial::Graphic graphic;
};

constexpr Word GRAPHICS[] = {
  {"false", GUI::Dial::NONE},
  {"none", GUI::Dial::NONE},
  {"true", GUI::Dial::BAR},
  {"bar", GUI::Dial::BAR},
  {"dial", GUI::Dial::DIAL}};

}  // namespace

auto GUI::PARSE::read(const String &value, Dial::Graphic &out) -> Status {
  for (const auto &row : ::GRAPHICS) {
    if (value != row.word) continue;
    out = row.graphic;
    return 0;
  }
  return 1;
}

auto GUI::TRANSLATE::graphic(Dial::Graphic graphic) -> STRING::Hot {
  for (const auto &row : ::GRAPHICS)
    if (row.graphic == graphic) return row.word;
  return ::GRAPHICS[0].word;
}
