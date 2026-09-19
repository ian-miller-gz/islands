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
#if SR_NETWORK_CARRIES(SR_UNIX) && SR_NETWORK_CARRIES(SR_TLS)
#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <network.hpp>
#include <test/streams/streams.internal.hpp>
#include <thread>

static auto dialed(const NETWORK::Tunnel &tunnel) -> Flag {
  NETWORK::Handle session = NETWORK::NONE;
  for (Whole i = 0; i < TRIES && session == NETWORK::NONE; i += 1) {
    session = NETWORK::SESSIONS::create(tunnel);
    if (session == NETWORK::NONE)
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  const Flag ok = echoed(session, "over-tls");
  if (session != NETWORK::NONE) NETWORK::SESSIONS::destroy(session);
  return ok;
}

static auto accepted(NETWORK::Handle listener) -> Flag {
  NETWORK::Handle session = NETWORK::NONE;
  for (Whole i = 0; i < TRIES && session == NETWORK::NONE; i += 1) {
    session = NETWORK::LISTENERS::accept(listener);
    if (session == NETWORK::NONE)
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  if (session == NETWORK::NONE) return false;
  String data;
  for (Whole i = 0; i < TRIES && data.find('\n') == String::npos; i += 1) {
    if (!NETWORK::SESSIONS::receive(session, data)) break;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  const Flag replied =
    data.find('\n') != String::npos && NETWORK::SESSIONS::send(session, data);
  NETWORK::SESSIONS::destroy(session);
  return replied;
}

Flag served(const NETWORK::Tunnel &tunnel) {
  pid_t child = fork();
  if (child == 0) _exit(dialed(tunnel) ? 0 : 1);
  NETWORK::Handle listener = NETWORK::LISTENERS::create(tunnel);
  const Flag replied = listener != NETWORK::NONE && accepted(listener);
  int status = 1;
  if (child > 0) waitpid(child, &status, 0);
  if (listener != NETWORK::NONE) NETWORK::LISTENERS::destroy(listener);
  return child > 0 && replied && WIFEXITED(status) && WEXITSTATUS(status) == 0;
}
#endif
