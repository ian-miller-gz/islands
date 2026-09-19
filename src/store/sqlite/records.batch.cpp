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

#include <store.hpp>
#include <store/sqlite/records.internal.hpp>
#include <store/sqlite/sqlite.internal.hpp>

namespace STORE {
namespace {

constexpr const char *SAVEPOINT = "islands_batch";

auto run(sqlite3 *connection, const String &sql) -> Flag {
  return sqlite3_exec(connection, sql.c_str(), nullptr, nullptr, nullptr) ==
         SQLITE_OK;
}

auto write(
  sqlite3 *connection, const String &identifier,
  const Vector<STORE::Row> &rows) -> Flag {
  sqlite3_stmt *statement = nullptr;
  String sql = "INSERT INTO " + identifier +
               "(key, fields) VALUES(?, ?) "
               "ON CONFLICT(key) DO UPDATE SET fields = excluded.fields;";
  if (
    sqlite3_prepare_v2(connection, sql.c_str(), -1, &statement, nullptr) !=
    SQLITE_OK)
    return false;

  Flag wrote = true;
  for (const STORE::Row &row : rows) {
    String blob = STORE::RECORDS::encode(row.fields);
    sqlite3_bind_text(
      statement, 1, row.key.data(), static_cast<int>(row.key.size()),
      SQLITE_TRANSIENT);
    sqlite3_bind_blob(
      statement, 2, blob.data(), static_cast<int>(blob.size()),
      SQLITE_TRANSIENT);
    wrote = sqlite3_step(statement) == SQLITE_DONE;
    sqlite3_reset(statement);
    if (!wrote) break;
  }
  sqlite3_finalize(statement);
  return wrote;
}

}  // namespace

auto put(Handle store, STRING::Hot table, const Vector<Row> &rows) -> Status {
  sqlite3 *connection = CONNECTION::GET::connection(store);
  const String identifier = RECORDS::quote(table);
  if (!connection || identifier.empty()) return 1;
  if (rows.empty()) return 0;

  RECORDS::create(connection, identifier);
  if (!run(connection, "SAVEPOINT " + String(SAVEPOINT) + ";")) {
    CONNECTION::GET::logger().error(
      "Cannot begin a batched write into " + identifier + ": " +
      sqlite3_errmsg(connection) + ".");
    return 1;
  }
  if (
    write(connection, identifier, rows) &&
    run(connection, "RELEASE " + String(SAVEPOINT) + ";"))
    return 0;

  CONNECTION::GET::logger().error(
    "Rolled back a batched write of " + std::to_string(rows.size()) +
    " rows into " + identifier + ": " + sqlite3_errmsg(connection) + ".");
  run(connection, "ROLLBACK TO " + String(SAVEPOINT) + ";");
  run(connection, "RELEASE " + String(SAVEPOINT) + ";");
  return 1;
}

}  // namespace STORE
