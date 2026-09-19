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

namespace RELAY {

Whole counter = 0;

THREADS::Signal outward{THREADS::EMPTY};
THREADS::Signal downward{THREADS::EMPTY};
THREADS::Signal upward{THREADS::EMPTY};
THREADS::Signal inward{THREADS::EMPTY};
THREADS::Signal primed{THREADS::EMPTY};

namespace {

void nest(THREADS::Token token) {
  while (true) {
    downward.acquire();
    if (token.stop_requested()) break;
    counter += 1;
    upward.release();
  }
  std::cout << "sub-worker: stop token seen, loop returns" << std::endl;
  upward.release();
}

}  // namespace

void work(THREADS::Token token) {
  primed.acquire();
  std::cout << "worker: took the primed signal without blocking" << std::endl;
  THREADS::Worker sub{nest};
  while (true) {
    outward.acquire();
    if (token.stop_requested()) break;
    counter += 1;
    downward.release();
    upward.acquire();
    counter += 1;
    inward.release();
  }
  sub.request_stop();
  downward.release();
  upward.acquire();
  std::cout << "worker: stop token seen, the nest stops with it" << std::endl;
  inward.release();
}

}  // namespace RELAY
