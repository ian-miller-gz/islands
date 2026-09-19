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
#pragma once
#include <sqlite/sqlite3.h>

#include <common/fields.hpp>
#include <store.hpp>

namespace STORE::RECORDS {

auto quote(STRING::Hot table) -> String;

void create(sqlite3 *connection, const String &identifier);

auto encode(const FIELDS::Map &record) -> String;
auto decode(const String &blob) -> FIELDS::Map;

inline auto blob(sqlite3_stmt *statement, int column) -> String {
  const void *bytes = sqlite3_column_blob(statement, column);
  int length = sqlite3_column_bytes(statement, column);
  return bytes ? String(static_cast<const char *>(bytes), length) : String();
}

}  // namespace STORE::RECORDS
