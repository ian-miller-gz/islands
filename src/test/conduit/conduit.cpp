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
#include <unistd.h>

#include <iostream>
#include <string>

#include <test/conduit/conduit.internal.hpp>

namespace {
auto check(const char *name, Flag ok) -> Status {
  std::cout << (ok ? "PASS " : "FAIL ") << name << std::endl;
  return ok ? 0 : 1;
}

auto local() -> Flag {
  const String name = named("conduit.local");
  const CONDUIT::Handle owner = CONDUIT::create(name, WIDTH, STRIDE);
  const CONDUIT::Handle peer = CONDUIT::open(name, WIDTH, STRIDE);
  if (owner == CONDUIT::NONE || peer == CONDUIT::NONE) return false;
  Flag ok = CONDUIT::GET::records(owner) == WIDTH;
  for (uint64_t seq = 0; seq < WIDTH; ++seq) {
    const Cell cell = sealed(seq);
    ok = ok && CONDUIT::write(owner, &cell);
  }
  Cell overflow = sealed(WIDTH);
  ok = ok && !CONDUIT::write(owner, &overflow);
  for (uint64_t seq = 0; seq < WIDTH; ++seq) {
    Cell cell;
    ok = ok && CONDUIT::read(peer, &cell) && intact(cell, seq);
  }
  Cell empty;
  ok = ok && !CONDUIT::read(peer, &empty);
  CONDUIT::destroy(peer);
  CONDUIT::destroy(owner);
  return ok;
}

auto pair() -> Flag {
  const String one = named("conduit.a");
  const String two = named("conduit.b");
  const CONDUIT::Handle first = CONDUIT::create(one, WIDTH, STRIDE);
  if (first == CONDUIT::NONE) return false;
  const Cell seed = sealed(7);
  Flag ok = CONDUIT::write(first, &seed);
  const CONDUIT::Handle second = CONDUIT::create(two, WIDTH, STRIDE);
  if (second == CONDUIT::NONE) {
    CONDUIT::destroy(first);
    return false;
  }
  const Cell other = sealed(9);
  ok = ok && CONDUIT::write(second, &other);
  Cell got;
  ok = ok && CONDUIT::read(first, &got) && intact(got, 7);
  ok = ok && CONDUIT::read(second, &got) && intact(got, 9);
  CONDUIT::destroy(second);
  CONDUIT::destroy(first);
  return ok;
}
}  // namespace

auto named(const char *base) -> String {
  return String(base) + "." + std::to_string(getpid());
}

int main(int count, char **words) {
  if (count > 2 && String(words[1]) == "peer") return produce(words[2]);
  Status status = check("local", local());
  status |= check("pair", pair());
  status |= check("wire", wire());
  return status;
}
