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
#include <store/sqlite/query.internal.hpp>
#include <store/sqlite/sqlite.internal.hpp>

namespace {

auto run(
  STORE::Handle store, STRING::Hot sql, const Vector<FIELDS::Value> &parameters,
  const STORE::Visitor &visit) -> Status {
  sqlite3 *connection = STORE::CONNECTION::GET::connection(store);
  if (!connection) return 1;
  sqlite3_stmt *statement = nullptr;
  if (
    sqlite3_prepare_v2(connection, sql ? sql : "", -1, &statement, nullptr) !=
    SQLITE_OK) {
    STORE::CONNECTION::GET::logger().error(
      "Refusing a malformed query: " + String(sqlite3_errmsg(connection)) +
      ".");
    return 1;
  }
  STORE::QUERY::bind(statement, parameters);
  int status = 0;
  while ((status = sqlite3_step(statement)) == SQLITE_ROW)
    visit(STORE::QUERY::row(statement));
  sqlite3_finalize(statement);
  return status == SQLITE_DONE ? 0 : 1;
}

}  // namespace

auto STORE::query(
  Handle store, STRING::Hot sql, const Vector<FIELDS::Value> &parameters,
  const Visitor &visit) -> Status {
  return run(store, sql, parameters, visit);
}

auto STORE::query(Handle store, STRING::Hot sql, const Visitor &visit)
  -> Status {
  return run(store, sql, {}, visit);
}

auto STORE::query(
  Handle store, STRING::Hot sql,
  const Vector<FIELDS::Value> &parameters) -> Vector<FIELDS::Map> {
  Vector<FIELDS::Map> rows;
  run(store, sql, parameters, [&rows](const FIELDS::Map &fields) {
    rows.push_back(fields);
  });
  return rows;
}

auto STORE::query(Handle store, STRING::Hot sql) -> Vector<FIELDS::Map> {
  return query(store, sql, Vector<FIELDS::Value>{});
}
