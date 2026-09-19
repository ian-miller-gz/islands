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
#pragma once

#include <functional>

#include <common.hpp>
#include <common/fields.hpp>

namespace STORE {

struct Handle {
  Whole id = 0;
};

struct Row {
  String key;
  FIELDS::Map fields;
};

using Visitor = std::function<void(const FIELDS::Map &row)>;

auto open(STRING::Hot bundle) -> Handle;

void close(Handle store);

void put(
  Handle store, STRING::Hot table, STRING::Hot key, const FIELDS::Map &record);

auto put(Handle store, STRING::Hot table, const Vector<Row> &rows) -> Status;

auto get(Handle store, STRING::Hot table, STRING::Hot key, FIELDS::Map &record)
  -> Flag;

void erase(Handle store, STRING::Hot table, STRING::Hot key);

auto scan(Handle store, STRING::Hot table, const FIELDS::Map &filter)
  -> Vector<Row>;
auto scan(Handle store, STRING::Hot table) -> Vector<Row>;

auto query(
  Handle store, STRING::Hot sql, const Vector<FIELDS::Value> &parameters,
  const Visitor &visit) -> Status;
auto query(Handle store, STRING::Hot sql, const Visitor &visit) -> Status;

auto query(
  Handle store, STRING::Hot sql,
  const Vector<FIELDS::Value> &parameters) -> Vector<FIELDS::Map>;
auto query(Handle store, STRING::Hot sql) -> Vector<FIELDS::Map>;

}  // namespace STORE

namespace STORE::GET {

auto version(Handle store) -> Whole;

}  // namespace STORE::GET

namespace STORE::SET {

void version(Handle store, Whole version);

}  // namespace STORE::SET

namespace STORE::SETTINGS {

void set(Handle store, STRING::Hot key, Flag value);
void set(Handle store, STRING::Hot key, Whole value);
void set(Handle store, STRING::Hot key, STRING::Cold value);

auto get(Handle store, STRING::Hot key, Flag &value) -> Flag;
auto get(Handle store, STRING::Hot key, Whole &value) -> Flag;
auto get(Handle store, STRING::Hot key, STRING::Cold &value) -> Flag;

}  // namespace STORE::SETTINGS
