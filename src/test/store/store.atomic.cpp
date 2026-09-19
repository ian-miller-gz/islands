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
#include "store.internal.hpp"

namespace {

constexpr Whole ROWS = 8;

void guarded(STORE::Handle store) {
  STORE::query(
    store,
    "CREATE TABLE guarded(key TEXT PRIMARY KEY CHECK(key <> 'row-2'), "
    "fields BLOB NOT NULL) WITHOUT ROWID;",
    [](const FIELDS::Map &) {});
}

void speak(STORE::Handle store, const char *sql) {
  STORE::query(store, sql, [](const FIELDS::Map &) {});
}

}  // namespace

auto atomic(STORE::Handle store) -> Status {
  using namespace STORE;
  guarded(store);
  Status status = check(
    "a refused record refuses the batch",
    put(store, "guarded", batch(ROWS)) != 0);
  status |= check(
    "the refused batch rolled its earlier records back",
    scan(store, "guarded").empty());

  speak(store, "BEGIN;");
  put(store, "joined", batch(ROWS));
  speak(store, "ROLLBACK;");
  status |= check(
    "the caller's rollback takes the batch", scan(store, "joined").empty());

  speak(store, "BEGIN;");
  put(store, "kept", batch(ROWS));
  put(store, "guarded", batch(ROWS));
  speak(store, "COMMIT;");
  status |= check(
    "a refused batch leaves the caller's work standing",
    scan(store, "kept").size() == ROWS);
  return status;
}
