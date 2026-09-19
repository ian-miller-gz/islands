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
#include <cctype>
#include <string>

#include <sqlite/sqlite3.h>

#include <store.hpp>
#include <store/sqlite/records.internal.hpp>
#include <store/sqlite/sqlite.internal.hpp>

auto STORE::RECORDS::quote(STRING::Hot table) -> String {
  auto &logger = CONNECTION::GET::logger();
  String name = table ? table : "";
  Flag valid = !name.empty();
  for (char c : name)
    valid = valid && (std::isalnum(static_cast<unsigned char>(c)) || c == '_');
  if (!valid) {
    logger.error(
      "Refusing a records table named \"" + name +
      "\": use letters, digits, and underscores.");
    return {};
  }
  if (name == "settings") {
    logger.error("Refusing the reserved `settings` table for records.");
    return {};
  }
  return "\"" + name + "\"";
}

void STORE::RECORDS::create(sqlite3 *connection, const String &identifier) {
  String ddl = "CREATE TABLE IF NOT EXISTS " + identifier +
               " (key TEXT PRIMARY KEY, fields BLOB NOT NULL) WITHOUT ROWID;";
  sqlite3_exec(connection, ddl.c_str(), nullptr, nullptr, nullptr);
}

void STORE::put(
  Handle store, STRING::Hot table, STRING::Hot key, const FIELDS::Map &record) {
  sqlite3 *connection = CONNECTION::GET::connection(store);
  const String identifier = RECORDS::quote(table);
  if (!connection || identifier.empty()) return;
  RECORDS::create(connection, identifier);

  String blob = RECORDS::encode(record);
  sqlite3_stmt *statement = nullptr;
  String sql = "INSERT INTO " + identifier +
               "(key, fields) VALUES(?, ?) "
               "ON CONFLICT(key) DO UPDATE SET fields = excluded.fields;";
  if (
    sqlite3_prepare_v2(connection, sql.c_str(), -1, &statement, nullptr) !=
    SQLITE_OK)
    return;
  sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
  sqlite3_bind_blob(
    statement, 2, blob.data(), static_cast<int>(blob.size()), SQLITE_TRANSIENT);
  sqlite3_step(statement);
  sqlite3_finalize(statement);
}

auto STORE::get(
  Handle store, STRING::Hot table, STRING::Hot key,
  FIELDS::Map &record) -> Flag {
  sqlite3 *connection = CONNECTION::GET::connection(store);
  const String identifier = RECORDS::quote(table);
  if (!connection || identifier.empty()) return false;

  sqlite3_stmt *statement = nullptr;
  String sql = "SELECT fields FROM " + identifier + " WHERE key = ?;";
  if (
    sqlite3_prepare_v2(connection, sql.c_str(), -1, &statement, nullptr) !=
    SQLITE_OK)
    return false;
  sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
  Flag found = sqlite3_step(statement) == SQLITE_ROW;
  if (found) record = RECORDS::decode(RECORDS::blob(statement, 0));
  sqlite3_finalize(statement);
  return found;
}

void STORE::erase(Handle store, STRING::Hot table, STRING::Hot key) {
  sqlite3 *connection = CONNECTION::GET::connection(store);
  const String identifier = RECORDS::quote(table);
  if (!connection || identifier.empty()) return;

  sqlite3_stmt *statement = nullptr;
  String sql = "DELETE FROM " + identifier + " WHERE key = ?;";
  if (
    sqlite3_prepare_v2(connection, sql.c_str(), -1, &statement, nullptr) !=
    SQLITE_OK)
    return;
  sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
  sqlite3_step(statement);
  sqlite3_finalize(statement);
}
