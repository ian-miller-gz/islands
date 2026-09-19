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
#include <filesystem>

#include <sqlite/sqlite3.h>

#include <store.hpp>
#include <store/home.internal.hpp>
#include <store/sqlite/sqlite.internal.hpp>

namespace {

auto leaf(STRING::Hot bundle) -> String {
  String name = bundle ? bundle : "";
  String safe;
  for (char c : name) safe += (c == '/' || c == '\\' || c == ':') ? '_' : c;
  return (safe.empty() || safe == "." || safe == "..") ? String("_") : safe;
}

}  // namespace

auto STORE::open(STRING::Hot bundle) -> Handle {
  auto &logger = CONNECTION::GET::logger();
  String directory = HOME::directory() + "/" + leaf(bundle);
  std::error_code error;
  std::filesystem::create_directories(directory, error);
  if (error) {
    logger.error(
      "Cannot create the store directory \"" + directory +
      "\": " + error.message() + ".");
    return {};
  }

  sqlite3 *connection = nullptr;
  String file = directory + "/store.db";
  if (sqlite3_open(file.c_str(), &connection) != SQLITE_OK) {
    logger.error(
      "Cannot open the store \"" + file + "\": " + sqlite3_errmsg(connection) +
      ".");
    sqlite3_close(connection);
    return {};
  }

  char *message = nullptr;
  const char *ddl =
    "CREATE TABLE IF NOT EXISTS settings ("
    "  key TEXT PRIMARY KEY, value TEXT NOT NULL) WITHOUT ROWID;";
  if (sqlite3_exec(connection, ddl, nullptr, nullptr, &message) != SQLITE_OK) {
    logger.error("Cannot prepare the settings table: " + String(message) + ".");
    sqlite3_free(message);
    sqlite3_close(connection);
    return {};
  }

  return CONNECTION::add(connection);
}
