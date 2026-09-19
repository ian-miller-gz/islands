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
#include <memory>

#include <conduit.hpp>
#include <threads/shared.hpp>

namespace CONDUIT {

constexpr Whole HEADER = 64;

using Counter = THREADS::Shared<uint64_t>;

struct Control {
  Counter head{0};
  Counter tail{0};
  uint64_t records = 0;
  uint64_t stride = 0;
};
static_assert(sizeof(Control) <= HEADER);
static_assert(Counter::is_always_lock_free);

struct Segment {
  String name;
  Flag owner = false;
  void *base = nullptr;
  Whole bytes = 0;
  void *mapping = nullptr;
  Control *control = nullptr;
  unsigned char *slots = nullptr;
};

auto table() -> Vector<std::unique_ptr<Segment>> &;

auto behind(Handle conduit) -> Segment *;

auto seat(Segment segment) -> Handle;

auto rounded(Whole records) -> Whole;

auto leaf(const String &name) -> String;
}  // namespace CONDUIT
