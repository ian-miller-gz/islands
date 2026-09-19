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

#include <cstdlib>
#include <filesystem>
#include <iostream>

auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

namespace {

auto home() -> std::filesystem::path {
  auto root = std::filesystem::temp_directory_path() / "islands-store-fixture";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);
  COMMON::PLATFORM::assign("XDG_DATA_HOME", root.string());
  return root;
}

}  // namespace

int main(int argc, char **argv) {
  using namespace STORE;
  Status status = 0;
  auto root = home();

  Handle store = open("fixture");
  status |= check("open answers a live store", store.id != 0);

  SETTINGS::set(store, "ready", Flag(true));
  SETTINGS::set(store, "count", Whole(42));
  SETTINGS::set(store, "label", STRING::Cold("hello"));
  Flag ready = false;
  Whole count = 0;
  STRING::Cold label;
  status |=
    check("flag round-trips", SETTINGS::get(store, "ready", ready) && ready);
  status |= check(
    "whole round-trips", SETTINGS::get(store, "count", count) && count == 42);
  status |= check(
    "text round-trips",
    SETTINGS::get(store, "label", label) && label == "hello");

  Whole fallback = 7;
  status |= check(
    "missing key keeps the default",
    !SETTINGS::get(store, "absent", fallback) && fallback == 7);

  FIELDS::Map wrote{
    {"name", String("Ada")},
    {"age", Whole(36)},
    {"score", Integer(-5)},
    {"ratio", Float(1.5f)},
    {"active", Flag(true)}};
  put(store, "people", "ada", wrote);
  FIELDS::Map read;
  status |= check(
    "record round-trips by key",
    get(store, "people", "ada", read) && read == wrote);

  FIELDS::Map seeded{{"sentinel", Whole(1)}};
  status |= check(
    "missing record keeps the default",
    !get(store, "people", "nobody", seeded) && seeded.size() == 1);
  erase(store, "people", "ada");
  FIELDS::Map gone;
  status |=
    check("erase removes the record", !get(store, "people", "ada", gone));
  put(store, "settings", "x", FIELDS::Map{{"v", Whole(1)}});
  status |= check(
    "records refuse the reserved settings table",
    scan(store, "settings").empty());

  put(
    store, "tickets", "a",
    FIELDS::Map{{"open", Flag(true)}, {"pri", Whole(1)}});
  put(
    store, "tickets", "b",
    FIELDS::Map{{"open", Flag(false)}, {"pri", Whole(2)}});
  put(
    store, "tickets", "c",
    FIELDS::Map{{"open", Flag(true)}, {"pri", Whole(2)}});
  status |= check(
    "unfiltered scan drains every record", scan(store, "tickets").size() == 3);
  status |= check(
    "a filter subset-matches",
    scan(store, "tickets", FIELDS::Map{{"open", Flag(true)}}).size() == 2);
  status |= check(
    "a filter matches the exact FIELDS type",
    scan(store, "tickets", FIELDS::Map{{"pri", Integer(2)}}).empty());

  status |= batched(store);
  status |= atomic(store);

  status |= check("a fresh store stamps version 0", GET::version(store) == 0);
  SET::version(store, 3);
  status |= check("the version stamp reads back", GET::version(store) == 3);

  Visitor sink = [](const FIELDS::Map &) {};
  status |= check(
    "a DDL statement runs to a clean Status",
    query(store, "CREATE TABLE hits(host TEXT, n INTEGER);", sink) == 0);
  status |= check(
    "an INSERT binds parameters by position",
    query(
      store, "INSERT INTO hits(host, n) VALUES(?, ?);",
      {String("alpha"), Integer(3)}, sink) == 0 &&
      query(
        store, "INSERT INTO hits(host, n) VALUES(?, ?);",
        {String("beta"), Integer(9)}, sink) == 0);

  Vector<FIELDS::Map> counted =
    query(store, "SELECT COUNT(*) AS rows FROM hits;");
  status |= check(
    "a drained aggregate answers one Integer row",
    counted.size() == 1 && counted[0].at("rows") == FIELDS::Value{Integer(2)});

  Whole visited = 0;
  query(
    store, "SELECT n FROM hits ORDER BY n;", [&](const FIELDS::Map &fields) {
      if (
        fields.at("n") == FIELDS::Value{Integer(3)} ||
        fields.at("n") == FIELDS::Value{Integer(9)})
        ++visited;
    });
  status |= check("the fan-out visitor sees every row", visited == 2);

  Vector<FIELDS::Map> matched =
    query(store, "SELECT host, n FROM hits WHERE n >= ?;", {Integer(5)});
  status |= check(
    "a parameterized filter matches by bound value",
    matched.size() == 1 &&
      matched[0].at("host") == FIELDS::Value{String("beta")} &&
      matched[0].at("n") == FIELDS::Value{Integer(9)});

  Vector<FIELDS::Map> safe = query(
    store, "SELECT host FROM hits WHERE host = ?;",
    {String("alpha'; DROP TABLE hits; --")});
  status |= check("a string parameter is data, not SQL", safe.empty());
  status |= check(
    "the injection attempt left the table intact",
    query(store, "SELECT COUNT(*) AS n FROM hits;").size() == 1);

  Vector<FIELDS::Map> setting = query(
    store, "SELECT value FROM settings WHERE key = ?;", {String("label")});
  status |= check(
    "the query tier reads the settings tier's table",
    setting.size() == 1 &&
      setting[0].at("value") == FIELDS::Value{String("hello")});

  status |= check(
    "malformed SQL refuses with Status",
    query(store, "SELECT bogus(", sink) != 0);
  status |= check(
    "malformed SQL drains nothing", query(store, "NOT SQL AT ALL").empty());
  status |= check(
    "a null store refuses the query", query(Handle{}, "SELECT 1;", sink) != 0);

  close(store);
  Handle again = open("fixture");
  Whole persisted = 0;
  status |= check(
    "settings persist across a reopen",
    SETTINGS::get(again, "count", persisted) && persisted == 42);
  FIELDS::Map survived;
  status |= check(
    "records persist across a reopen",
    get(again, "tickets", "c", survived) &&
      survived == FIELDS::Map{{"open", Flag(true)}, {"pri", Whole(2)}});
  status |= check(
    "the version stamp persists across a reopen", GET::version(again) == 3);
  Vector<FIELDS::Map> reopened =
    query(again, "SELECT COUNT(*) AS n FROM hits;");
  status |= check(
    "a query-tier table persists across a reopen",
    reopened.size() == 1 && reopened[0].at("n") == FIELDS::Value{Integer(2)});

  Handle other = open("other");
  Whole leaked = 0;
  status |= check(
    "a sibling store is isolated", !SETTINGS::get(other, "count", leaked));
  status |=
    check("a sibling store shares no records", scan(other, "tickets").empty());
  close(other);

  if (argc > 1 && String(argv[1]) == "rate") rate(again);
  close(again);

  std::filesystem::remove_all(root);
  return status;
}
