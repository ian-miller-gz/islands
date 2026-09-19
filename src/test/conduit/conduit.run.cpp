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
#include <thread>

#include <test/conduit/conduit.internal.hpp>

auto sealed(uint64_t seq) -> Cell {
  return {seq, seq * 0x9E3779B97F4A7C15ull + 0xD1B54A32D192ED03ull};
}

auto intact(const Cell &cell, uint64_t seq) -> Flag {
  return cell.seq == seq && cell.sum == sealed(seq).sum;
}

auto produce(const String &name) -> Status {
  const CONDUIT::Handle peer = CONDUIT::open(name, WIDTH, STRIDE);
  if (peer == CONDUIT::NONE) return 2;
  for (uint64_t seq = 0; seq < RUN; ++seq) {
    const Cell cell = sealed(seq);
    uint64_t spins = 0;
    while (!CONDUIT::write(peer, &cell))
      if (++spins > SPIN)
        return 3;
      else
        std::this_thread::yield();
  }
  CONDUIT::destroy(peer);
  return 0;
}

auto drained(CONDUIT::Handle owner) -> Flag {
  Flag ok = true;
  for (uint64_t seq = 0; ok && seq < RUN; ++seq) {
    Cell cell;
    uint64_t spins = 0;
    while (ok && !CONDUIT::read(owner, &cell)) ok = ++spins <= SPIN;
    ok = ok && intact(cell, seq);
  }
  return ok;
}
