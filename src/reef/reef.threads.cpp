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
#include <exception>
#include <utility>

#include <cartridge.hpp>
#include <common.hpp>
#include <logger.hpp>
#include <metrics.hpp>
#include <network.hpp>
#include <reef/halt.hpp>
#include <reef/reef.internal.hpp>
#include <threads.hpp>
#include <topics.hpp>
static const String category = "~/reef::";

static THREADS::Worker worker;
static std::exception_ptr thrown;

static void work(THREADS::Token token) {
  try {
    while (true) {
      NETWORK::wait();
      if (token.stop_requested()) break;
      METRICS::frame();
      METRICS::memory();
      METRICS::Scope span("reef.serve");
      REQUIREMENTS::process();
      RELATIONS::process();
      CLOCK::stamp();
      TOPICS::deliver(TOPICS::now());
      TOPICS::relay();
      CARTRIDGE::serve();
    }
  } catch (...) {
    thrown = std::current_exception();
    REEF::HALT::summon();
  }
}

void REEF::detach() {
  LOGGER::get(category + "detach").debug("Serve worker detaching.");
  worker = THREADS::Worker(work);
}

void REEF::join() {
  if (!worker.joinable()) return;
  worker.request_stop();
  NETWORK::wake();
  worker.join();
  LOGGER::get(category + "join").debug("Serve worker joined.");
  if (auto error = std::exchange(thrown, nullptr))
    std::rethrow_exception(error);
}
