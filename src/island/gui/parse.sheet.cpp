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

static auto imported(
  GUI::PARSE::Cursor &cursor, const String &text,
  const String &site) -> Status {
  using namespace GUI::PARSE;
  if (text.empty()) return 0;
  Whole level = 0;
  if (measure(text, level) != 0)
    return refuse(site + ": indentation must be spaces, two per level");
  Vector<String> fields;
  if (split(text, fields) != 0) return refuse(site + ": unterminated quote");
  if (fields.empty()) return 0;
  if (fields.front() != "style")
    return refuse(site + ": a sheet holds style definitions only");
  return define(cursor, fields, level, site);
}

static auto dressed(const String &path) -> String {
  if (GUI::GET::theme() != GUI::DARK) return path;
  if (!path.ends_with(".gui")) return path;
  const String dark = path.substr(0, path.size() - 4) + ".dark.gui";
  IO::STREAMS::Input sibling(dark);
  return sibling ? dark : path;
}

auto GUI::PARSE::include(
  Cursor &cursor, const Vector<String> &fields, Whole level,
  const String &site) -> Status {
  if (level != 0) return refuse(site + ": the include is top-level");
  if (fields.size() != 1)
    return refuse(site + ": the include stands alone on its line");
  const String path = ::dressed(fields.front().substr(STYLE.size()));
  IO::STREAMS::Input file(path);
  if (!file) return refuse(site + ": cannot read the sheet '" + path + "'");
  String line;
  for (Whole number = 1; std::getline(file, line); number += 1) {
    const auto at = path + ":" + std::to_string(number);
    if (::imported(cursor, strip(line), at) != 0) return 1;
  }
  return 0;
}
