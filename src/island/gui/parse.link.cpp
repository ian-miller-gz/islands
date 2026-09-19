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
#include <island/gui/nga.hpp>

auto GUI::PARSE::link(
  Cursor &cursor, const Vector<String> &fields, Whole level,
  const String &site) -> Status {
  constexpr Whole FROM = 1;
  constexpr Whole TO = 2;
  constexpr Whole LABEL = 3;
  if (level != 0) return refuse(site + ": link lines are top-level");
  if (fields.size() <= TO) return refuse(site + ": link without a pair");
  if (fields.size() > LABEL + 1)
    return refuse(site + ": link past its label '" + fields[LABEL + 1] + "'");
  const NGA::Label label{fields.size() > LABEL ? fields[LABEL].c_str() : ""};
  if (
    NGA::connect(
      cursor.document, fields[FROM].c_str(), fields[TO].c_str(), label) != 0)
    return refuse(
      site + ": link refused '" + fields[FROM] + "' '" + fields[TO] + "'");
  return 0;
}
