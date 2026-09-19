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
#include <island/gui/backend.hpp>
#include <island/gui/gui.internal.hpp>
#include <island/gui/parse.internal.hpp>

static auto inherit(
  GUI::PARSE::Cursor &cursor, const String &field, Vector<String> &tokens,
  const String &site) -> Status {
  const String name = field.substr(GUI::PARSE::STYLE.size());
  const auto &styles = GUI::STATE::documents[cursor.document].styles;
  const auto found = styles.find(name);
  if (found == styles.end())
    return GUI::PARSE::refuse(site + ": undefined style '" + name + "'");
  tokens.insert(tokens.end(), found->second.begin(), found->second.end());
  return 0;
}

auto GUI::PARSE::define(
  GUI::PARSE::Cursor &cursor, const Vector<String> &fields, Whole level,
  const String &site) -> Status {
  constexpr Whole NAME = 1;
  if (level != 0)
    return GUI::PARSE::refuse(site + ": style definitions are top-level");
  if (fields.size() <= NAME)
    return GUI::PARSE::refuse(site + ": style without a name");
  Vector<String> tokens;
  for (Whole index = NAME + 1; index < fields.size(); index += 1)
    if (
      fields[index].starts_with(GUI::PARSE::STYLE) &&
      inherit(cursor, fields[index], tokens, site) != 0)
      return 1;
  for (Whole index = NAME + 1; index < fields.size(); index += 1)
    if (!fields[index].starts_with(GUI::PARSE::STYLE))
      tokens.push_back(fields[index]);
  GUI::STATE::documents[cursor.document].styles[fields[NAME]] = tokens;
  return 0;
}

static auto build(
  GUI::PARSE::Cursor &cursor, const Vector<String> &fields, Whole level,
  const String &site) -> Status {
  constexpr Whole ID = 1;
  if (!GUI::BACKEND::known(fields.front().c_str()))
    return GUI::PARSE::refuse(site + ": unknown kind '" + fields.front() + "'");
  if (fields.size() <= ID)
    return GUI::PARSE::refuse(site + ": widget without an id");
  if (level > cursor.stack.size())
    return GUI::PARSE::refuse(site + ": indent jumps more than one level");
  cursor.stack.resize(level);
  const String parent =
    level == 0 ? String(GUI::NODES::ROOT) : cursor.stack[level - 1];
  if (
    GUI::NODES::create(
      cursor.document, parent.c_str(), fields.front().c_str(),
      fields[ID].c_str()) != 0)
    return GUI::PARSE::refuse(site + ": node refused '" + fields[ID] + "'");
  cursor.stack.push_back(fields[ID]);
  return GUI::PARSE::dress(cursor, fields, site);
}

auto GUI::PARSE::interpret(
  Cursor &cursor, const String &text, const String &site) -> Status {
  Whole level = 0;
  if (measure(text, level) != 0)
    return refuse(site + ": indentation must be spaces, two per level");
  Vector<String> fields;
  if (split(text, fields) != 0) return refuse(site + ": unterminated quote");
  if (fields.empty()) return 0;
  if (fields.front() == "style") return define(cursor, fields, level, site);
  if (fields.front() == "link") return link(cursor, fields, level, site);
  if (fields.front().starts_with(STYLE))
    return include(cursor, fields, level, site);
  return build(cursor, fields, level, site);
}
