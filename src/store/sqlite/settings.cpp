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
#include <cstdlib>

#include <sqlite/sqlite3.h>

#include <store.hpp>
#include <store/sqlite/sqlite.internal.hpp>

namespace {

void write(sqlite3 *connection, STRING::Hot key, const String &value) {
  sqlite3_stmt *statement = nullptr;
  const char *sql =
    "INSERT INTO settings(key, value) VALUES(?, ?) "
    "ON CONFLICT(key) DO UPDATE SET value = excluded.value;";
  if (sqlite3_prepare_v2(connection, sql, -1, &statement, nullptr) != SQLITE_OK)
    return;
  sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, value.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_step(statement);
  sqlite3_finalize(statement);
}

auto read(sqlite3 *connection, STRING::Hot key, String &out) -> Flag {
  sqlite3_stmt *statement = nullptr;
  const char *sql = "SELECT value FROM settings WHERE key = ?;";
  if (sqlite3_prepare_v2(connection, sql, -1, &statement, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
  Flag found = sqlite3_step(statement) == SQLITE_ROW;
  if (found)
    out = reinterpret_cast<const char *>(sqlite3_column_text(statement, 0));
  sqlite3_finalize(statement);
  return found;
}

}  // namespace

namespace STORE::SETTINGS {

void set(Handle store, STRING::Hot key, Flag value) {
  if (sqlite3 *connection = CONNECTION::GET::connection(store))
    write(connection, key, value ? "1" : "0");
}

void set(Handle store, STRING::Hot key, Whole value) {
  if (sqlite3 *connection = CONNECTION::GET::connection(store))
    write(connection, key, std::to_string(value));
}

void set(Handle store, STRING::Hot key, STRING::Cold value) {
  if (sqlite3 *connection = CONNECTION::GET::connection(store))
    write(connection, key, value);
}

auto get(Handle store, STRING::Hot key, Flag &value) -> Flag {
  sqlite3 *connection = CONNECTION::GET::connection(store);
  String text;
  if (!connection || !read(connection, key, text)) return false;
  value = text == "1";
  return true;
}

auto get(Handle store, STRING::Hot key, Whole &value) -> Flag {
  sqlite3 *connection = CONNECTION::GET::connection(store);
  String text;
  if (!connection || !read(connection, key, text)) return false;
  value = static_cast<Whole>(std::strtoul(text.c_str(), nullptr, 10));
  return true;
}

auto get(Handle store, STRING::Hot key, STRING::Cold &value) -> Flag {
  sqlite3 *connection = CONNECTION::GET::connection(store);
  String text;
  if (!connection || !read(connection, key, text)) return false;
  value = text;
  return true;
}

}  // namespace STORE::SETTINGS
