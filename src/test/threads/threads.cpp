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
#include "threads.internal.hpp"
#include <iostream>

namespace {

void lap(Whole number) {
  RELAY::outward.release();
  RELAY::inward.acquire();
  std::cout << "main: lap " << number << " counter " << RELAY::counter
            << std::endl;
}

}  // namespace

int main() {
  std::cout << "main: primed released before the worker exists" << std::endl;
  RELAY::primed.release();
  THREADS::Worker worker{RELAY::work};
  for (Whole number = 1; number <= RELAY::LAPS; number += 1) lap(number);
  worker.request_stop();
  RELAY::outward.release();
  RELAY::inward.acquire();
  const Flag ok = RELAY::counter == RELAY::LAPS * RELAY::STEPS;
  std::cout << "threads relay: " << (ok ? "PASS" : "FAIL") << std::endl;
  return ok ? 0 : 1;
}
