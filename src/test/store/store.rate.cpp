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

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>

namespace {

constexpr Whole COMMITS = 60;
constexpr Whole PAYLOAD = 150;
constexpr Whole SIZES[] = {10, 100, 1000};

auto row(Whole index) -> STORE::Row {
  FIELDS::Map fields{
    {"stamp", Whole(index)},
    {"bid", Float(1.0f)},
    {"ask", Float(2.0f)},
    {"note", String(PAYLOAD, 'x')}};
  return {"row-" + std::to_string(index), fields};
}

auto rows(Whole count) -> Vector<STORE::Row> {
  Vector<STORE::Row> records;
  for (Whole index = 0; index < count; index += 1)
    records.push_back(row(index));
  return records;
}

void report(const String &lane, Whole count, Float seconds) {
  std::cout << "  " << std::left << std::setw(22) << lane << std::right
            << std::setw(7) << count << " rows  " << std::fixed
            << std::setprecision(5) << seconds / Float(count) << " s/row  "
            << std::setprecision(1) << Float(count) / seconds << " rows/s"
            << std::endl;
}

auto singly(STORE::Handle store, const Vector<STORE::Row> &records) -> Float {
  auto start = std::chrono::steady_clock::now();
  for (const STORE::Row &record : records)
    STORE::put(store, "singly", record.key.c_str(), record.fields);
  return std::chrono::duration<Float>(std::chrono::steady_clock::now() - start)
    .count();
}

auto shared(
  STORE::Handle store, const String &table, const Vector<STORE::Row> &records,
  Whole size) -> Float {
  auto start = std::chrono::steady_clock::now();
  for (Whole first = 0; first < records.size(); first += size)
    STORE::put(
      store, table.c_str(),
      Vector<STORE::Row>{
        records.begin() + first, records.begin() + first + size});
  return std::chrono::duration<Float>(std::chrono::steady_clock::now() - start)
    .count();
}

}  // namespace

void rate(STORE::Handle store) {
  const char *home = std::getenv("XDG_DATA_HOME");
  std::cout << "rate: " << COMMITS << " durable commits a lane, each row four "
            << "fields with a " << PAYLOAD << "-byte text value." << std::endl
            << "rate: on whatever device carries " << (home ? home : "?")
            << " — state it beside any figure taken from here." << std::endl;

  report("single-row put", COMMITS, singly(store, rows(COMMITS)));
  for (Whole size : SIZES) {
    String table = "shared_" + std::to_string(size);
    Vector<STORE::Row> records = rows(COMMITS * size);
    report(
      "batch of " + std::to_string(size), COMMITS * size,
      shared(store, table, records, size));
  }
}
