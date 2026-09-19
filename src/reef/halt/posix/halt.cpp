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
#include <csignal>
#include <signal.h>
#include <unistd.h>

#include <reef/halt.hpp>

namespace REEF::HALT {
namespace {

auto shutdowns() -> sigset_t {
  sigset_t held;
  sigemptyset(&held);
  sigaddset(&held, SIGINT);
  sigaddset(&held, SIGTERM);
  return held;
}

volatile std::sig_atomic_t asked = 0;

void answer(int) { asked = 1; }

}  // namespace

void arm(Flag threaded) {
  const auto held = shutdowns();
  sigprocmask(SIG_BLOCK, &held, nullptr);
  if (threaded) return;
  std::signal(SIGINT, answer);
  std::signal(SIGTERM, answer);
}

auto halted() -> Flag { return asked != 0; }

void await() {
  const auto held = shutdowns();
  int caught = 0;
  sigwait(&held, &caught);
}

void summon() { ::kill(::getpid(), SIGTERM); }

}  // namespace REEF::HALT
