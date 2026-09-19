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
constexpr Whole REPLACED = 300;

auto holds(STORE::Handle store, const char *table, const char *key, Whole index)
  -> Flag {
  FIELDS::Map record;
  return STORE::get(store, table, key, record) &&
         record == FIELDS::Map{{"index", Whole(index)}};
}

auto landing(STORE::Handle store) -> Status {
  using namespace STORE;
  Status status = check(
    "a batch commits and answers a clean Status",
    put(store, "tape", batch(ROWS)) == 0);
  status |= check(
    "every record in the batch landed", scan(store, "tape").size() == ROWS);
  status |= check(
    "a batched record reads back by key", holds(store, "tape", "row-3", 3));

  Vector<Row> again = batch(ROWS + 1);
  again[3].fields = FIELDS::Map{{"index", Whole(REPLACED)}};
  status |= check("a batch upserts", put(store, "tape", again) == 0);
  status |= check(
    "the replaced record carries the second write",
    holds(store, "tape", "row-3", REPLACED));
  status |= check(
    "the batch extended the table", scan(store, "tape").size() == ROWS + 1);
  status |= check(
    "a drained scan writes back as a batch",
    put(store, "copy", scan(store, "tape")) == 0 &&
      scan(store, "copy").size() == ROWS + 1);
  return status;
}

auto refusing(STORE::Handle store) -> Status {
  using namespace STORE;
  Status status =
    check("an empty batch answers clean", put(store, "spare", {}) == 0);
  status |=
    check("an empty batch creates no table", scan(store, "spare").empty());
  status |= check(
    "a batch refuses the reserved settings table",
    put(store, "settings", batch(ROWS)) != 0);
  status |= check(
    "a batch refuses an invalid table name",
    put(store, "no-dashes", batch(ROWS)) != 0);
  status |= check(
    "a null store refuses the batch", put(Handle{}, "tape", batch(ROWS)) != 0);
  return status;
}

}  // namespace

auto batch(Whole count) -> Vector<STORE::Row> {
  Vector<STORE::Row> records;
  for (Whole index = 0; index < count; index += 1)
    records.push_back(
      {"row-" + std::to_string(index), FIELDS::Map{{"index", Whole(index)}}});
  return records;
}

auto batched(STORE::Handle store) -> Status {
  return landing(store) | refusing(store);
}
