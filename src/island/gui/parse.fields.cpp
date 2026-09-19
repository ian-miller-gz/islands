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

auto GUI::PARSE::strip(const String &line) -> String {
  Flag quoted = false;
  auto cut = line.size();
  for (Whole index = 0; index < line.size(); index += 1) {
    if (line[index] == '"') quoted = !quoted;
    if (quoted || line[index] != '#') continue;
    if (index == 0 || line[index - 1] == ' ' || line[index - 1] == '\t') {
      cut = index;
      break;
    }
  }
  const auto text = line.substr(0, cut);
  const auto end = text.find_last_not_of(" \t\r");
  return end == String::npos ? "" : text.substr(0, end + 1);
}

auto GUI::PARSE::split(const String &line, Vector<String> &fields) -> Status {
  fields.clear();
  String field;
  Flag quoted = false;
  Flag present = false;
  for (const auto glyph : line) {
    if (glyph == '"') {
      quoted = !quoted;
      present = true;
      continue;
    }
    if (glyph == ' ' && !quoted) {
      if (present) fields.push_back(field);
      field.clear();
      present = false;
      continue;
    }
    field += glyph;
    present = true;
  }
  if (present) fields.push_back(field);
  return quoted ? 1 : 0;
}

auto GUI::PARSE::measure(const String &line, Whole &level) -> Status {
  Whole spaces = 0;
  while (spaces < line.size() && line[spaces] == ' ') spaces += 1;
  if (spaces < line.size() && line[spaces] == '\t') return 1;
  if (spaces % INDENT != 0) return 1;
  level = spaces / INDENT;
  return 0;
}
