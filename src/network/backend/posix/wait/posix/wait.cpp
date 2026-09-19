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
#include <network/backend/selection.hpp>
#if SR_NETWORK_BACKEND != SR_NONE
#include <poll.h>
#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <network/backend/posix/listeners.internal.hpp>
#include <network/backend/posix/sessions.internal.hpp>
#include <network/wait.hpp>

Flag NETWORK::threads = false;

using Tick = uint64_t;
constexpr Tick RING = 1;

static auto bell() -> NETWORK::Descriptor {
  static const NETWORK::Descriptor rung =
    ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  return rung;
}

static void gather(Vector<pollfd> &queries, NETWORK::Descriptor descriptor) {
  if (descriptor == NETWORK::CLOSED) return;
  queries.push_back({.fd = descriptor, .events = POLLIN, .revents = 0});
}

void NETWORK::wait() {
  Vector<pollfd> queries;
  if (threads) gather(queries, bell());
  for (const auto &entry : LISTENERS::entries)
    gather(queries, entry.descriptor);
  for (auto descriptor : SESSIONS::descriptors) gather(queries, descriptor);
  if (threads) {
    NETWORK::poll(queries.data(), queries.size(), -1);
    Tick tick;
    static_cast<void>(::read(bell(), &tick, sizeof tick));
    return;
  }
  sigset_t open;
  sigemptyset(&open);
  ppoll(queries.data(), queries.size(), nullptr, &open);
}

void NETWORK::wake() { static_cast<void>(::write(bell(), &RING, sizeof RING)); }
#endif
