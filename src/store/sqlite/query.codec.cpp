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
#include <sqlite/sqlite3.h>

#include <store/sqlite/query.internal.hpp>
#include <store/sqlite/records.internal.hpp>

namespace {

auto value(sqlite3_stmt *statement, int column) -> FIELDS::Value {
  switch (sqlite3_column_type(statement, column)) {
    case SQLITE_INTEGER:
      return Integer(sqlite3_column_int64(statement, column));
    case SQLITE_FLOAT:
      return Float(sqlite3_column_double(statement, column));
    case SQLITE_NULL:
      return String();
    default:
      return STORE::RECORDS::blob(statement, column);
  }
}

}  // namespace

void STORE::QUERY::bind(
  sqlite3_stmt *statement, const Vector<FIELDS::Value> &parameters) {
  int index = 1;
  for (const FIELDS::Value &value : parameters) {
    switch (value.index()) {
      case 0:
        sqlite3_bind_int(statement, index, std::get<Flag>(value) ? 1 : 0);
        break;
      case 1:
        sqlite3_bind_int64(statement, index, std::get<Integer>(value));
        break;
      case 2:
        sqlite3_bind_int64(
          statement, index, static_cast<sqlite3_int64>(std::get<Whole>(value)));
        break;
      case 3:
        sqlite3_bind_double(statement, index, std::get<Float>(value));
        break;
      case 4: {
        const String &text = std::get<String>(value);
        sqlite3_bind_text(
          statement, index, text.data(), static_cast<int>(text.size()),
          SQLITE_TRANSIENT);
        break;
      }
    }
    ++index;
  }
}

auto STORE::QUERY::row(sqlite3_stmt *statement) -> FIELDS::Map {
  FIELDS::Map fields;
  for (int c = 0, columns = sqlite3_column_count(statement); c < columns; ++c)
    if (const char *name = sqlite3_column_name(statement, c))
      fields[name] = value(statement, c);
  return fields;
}
