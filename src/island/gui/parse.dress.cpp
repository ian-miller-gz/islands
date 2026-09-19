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
#include <island/gui/gui.internal.hpp>
#include <island/gui/parse.internal.hpp>

static constexpr Whole KEYS = 2;

static auto assign(
  GUI::PARSE::Cursor &cursor, const String &id, const String &token,
  const String &site) -> Status {
  const auto mark = token.find('=');
  if (
    mark != String::npos && mark != 0 &&
    GUI::PARSE::apply(
      cursor.document, id.c_str(), token.substr(0, mark),
      token.substr(mark + 1)) == 0)
    return 0;
  return GUI::PARSE::refuse(site + ": refused property '" + token + "'");
}

static auto expand(
  GUI::PARSE::Cursor &cursor, const String &name, const String &id,
  const String &site) -> Status {
  const auto &styles = GUI::STATE::documents[cursor.document].styles;
  const auto found = styles.find(name);
  if (found == styles.end())
    return GUI::PARSE::refuse(site + ": undefined style '" + name + "'");
  for (const auto &token : found->second)
    if (assign(cursor, id, token, site) != 0) return 1;
  return 0;
}

auto GUI::PARSE::dress(
  Cursor &cursor, const Vector<String> &fields, const String &site) -> Status {
  const auto &id = fields[1];
  for (Whole index = KEYS; index < fields.size(); index += 1)
    if (
      fields[index].starts_with(STYLE) &&
      expand(cursor, fields[index].substr(STYLE.size()), id, site) != 0)
      return 1;
  for (Whole index = KEYS; index < fields.size(); index += 1)
    if (
      !fields[index].starts_with(STYLE) &&
      assign(cursor, id, fields[index], site) != 0)
      return 1;
  return 0;
}
