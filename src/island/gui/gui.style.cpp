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
#include <logger.hpp>
static const String category = "~/island/gui::";

static auto assign(GUI::Handle document, STRING::Hot id, const String &token)
  -> Status {
  const auto mark = token.find('=');
  if (mark == String::npos || mark == 0) return 1;
  return GUI::PARSE::apply(
    document, id, token.substr(0, mark), token.substr(mark + 1));
}

auto GUI::set(Handle document, STRING::Hot id, Style style) -> Status {
  Whole node = PARENTLESS;
  if (find(document, id, node) != 0) return 1;
  const auto &styles = STATE::documents[document].styles;
  const auto found = styles.find(style.name);
  auto &logger = LOGGER::get(category + "styles");
  if (found == styles.end()) {
    logger.warn(
      "Unknown style '" + String(style.name) + "' on '" + String(id) + "'");
    return 1;
  }
  for (const auto &token : found->second)
    if (::assign(document, id, token) != 0) {
      logger.warn(
        "Style '" + String(style.name) + "' refused '" + token + "' on '" +
        String(id) + "'");
      return 1;
    }
  return 0;
}

auto GUI::worn(const Document &entry, STRING::Hot name) -> Properties {
  const auto found = entry.styles.find(name);
  if (found == entry.styles.end()) return {};
  Properties properties;
  for (const auto &token : found->second) {
    const auto mark = token.find('=');
    if (mark == String::npos || mark == 0) continue;
    PARSE::apply(properties, token.substr(0, mark), token.substr(mark + 1));
  }
  return properties;
}

auto GUI::GET::style(Handle document, STRING::Hot name) -> Properties {
  if (!live(document)) return {};
  return worn(STATE::documents[document], name);
}
