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
#include <island/process.hpp>
#include <island/process.internal.hpp>
#include <common.hpp>
#include <logger.hpp>
#include <threads.hpp>
#include <exception>
#include <utility>
static const String category = "~/island/process::";

static THREADS::Worker worker;
static THREADS::Signal ready{THREADS::EMPTY};
static THREADS::Signal done{THREADS::EMPTY};
static std::exception_ptr thrown;

static void work(THREADS::Token token) {
  while (true) {
    ready.acquire();
    if (token.stop_requested()) break;
    try {
      ISLAND::frame();
    } catch (...) {
      thrown = std::current_exception();
    }
    done.release();
  }
}

void ISLAND::detach() {
  worker = THREADS::Worker(work);
  LOGGER::get(category + "detach").debug("Frame worker detached.");
}

void ISLAND::alternate() {
  ready.release();
  done.acquire();
  if (thrown) join();
}

void ISLAND::join() {
  if (!worker.joinable()) return;
  worker.request_stop();
  ready.release();
  worker.join();
  LOGGER::get(category + "join").debug("Frame worker joined.");
  if (auto error = std::exchange(thrown, nullptr))
    std::rethrow_exception(error);
}
