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
#include <string>

#include <sqlite/sqlite3.h>

#include <store.hpp>
#include <store/sqlite/sqlite.internal.hpp>

auto STORE::GET::version(Handle store) -> Whole {
  sqlite3 *connection = CONNECTION::GET::connection(store);
  if (!connection) return 0;
  sqlite3_stmt *statement = nullptr;
  if (
    sqlite3_prepare_v2(
      connection, "PRAGMA user_version;", -1, &statement, nullptr) != SQLITE_OK)
    return 0;
  Whole version = sqlite3_step(statement) == SQLITE_ROW
                    ? static_cast<Whole>(sqlite3_column_int64(statement, 0))
                    : 0;
  sqlite3_finalize(statement);
  return version;
}

void STORE::SET::version(Handle store, Whole version) {
  if (sqlite3 *connection = CONNECTION::GET::connection(store))
    sqlite3_exec(
      connection,
      ("PRAGMA user_version = " + std::to_string(version) + ";").c_str(),
      nullptr, nullptr, nullptr);
}
