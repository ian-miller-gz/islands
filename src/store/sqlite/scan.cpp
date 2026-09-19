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
#include <utility>

#include <sqlite/sqlite3.h>

#include <store.hpp>
#include <store/sqlite/records.internal.hpp>
#include <store/sqlite/sqlite.internal.hpp>

namespace {

auto match(const FIELDS::Map &record, const FIELDS::Map &filter) -> Flag {
  for (const auto &[name, value] : filter) {
    auto found = record.find(name);
    if (found == record.end() || !(found->second == value)) return false;
  }
  return true;
}

}  // namespace

auto STORE::scan(Handle store, STRING::Hot table, const FIELDS::Map &filter)
  -> Vector<Row> {
  Vector<Row> rows;
  sqlite3 *connection = CONNECTION::GET::connection(store);
  const String identifier = RECORDS::quote(table);
  if (!connection || identifier.empty()) return rows;

  sqlite3_stmt *statement = nullptr;
  String sql = "SELECT key, fields FROM " + identifier + ";";
  if (
    sqlite3_prepare_v2(connection, sql.c_str(), -1, &statement, nullptr) !=
    SQLITE_OK)
    return rows;
  while (sqlite3_step(statement) == SQLITE_ROW) {
    String key =
      reinterpret_cast<const char *>(sqlite3_column_text(statement, 0));
    FIELDS::Map fields = RECORDS::decode(RECORDS::blob(statement, 1));
    if (match(fields, filter))
      rows.push_back({std::move(key), std::move(fields)});
  }
  sqlite3_finalize(statement);
  return rows;
}

auto STORE::scan(Handle store, STRING::Hot table) -> Vector<Row> {
  return scan(store, table, FIELDS::Map{});
}
