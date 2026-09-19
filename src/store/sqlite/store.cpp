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
#include <store/sqlite/sqlite.internal.hpp>

#define LOGGER_CATEGORY "~/store"

namespace {

Vector<sqlite3 *> connections;

}  // namespace

auto STORE::CONNECTION::GET::connection(Handle store) -> sqlite3 * {
  return (store.id && store.id <= connections.size())
           ? connections[store.id - 1]
           : nullptr;
}

auto STORE::CONNECTION::add(sqlite3 *connection) -> Handle {
  connections.push_back(connection);
  return {static_cast<Whole>(connections.size())};
}

auto STORE::CONNECTION::GET::logger() -> LOGGER::Category & {
  return LOGGER::get(LOGGER_CATEGORY);
}

void STORE::close(Handle store) {
  if (sqlite3 *connection = CONNECTION::GET::connection(store)) {
    sqlite3_close(connection);
    connections[store.id - 1] = nullptr;
  }
}
