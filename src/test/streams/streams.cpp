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
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <network.hpp>
#include <test/streams/streams.internal.hpp>
#include <thread>

Flag echoed(NETWORK::Handle session, const String &message) {
  if (session == NETWORK::NONE) return false;
  if (!NETWORK::SESSIONS::send(session, message + "\n")) return false;
  String data;
  for (Whole i = 0; i < TRIES; i += 1) {
    const Flag alive = NETWORK::SESSIONS::receive(session, data);
    if (data.find(message) != String::npos) return true;
    if (!alive) return false;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  return false;
}

Status check(STRING::Hot name, Flag passed) {
  std::cout << "streams " << name << ": " << (passed ? "PASS" : "FAIL")
            << std::endl;
  return passed ? 0 : 1;
}

#if SR_NETWORK_CARRIES(SR_UNIX) && SR_NETWORK_CARRIES(SR_TCP)
// A dial is settled by glances: the session stands DIALING until the peer
// answers, then OPEN; a refused dial closes. Neither glance waits.
static auto settled(NETWORK::Handle session, NETWORK::SESSIONS::State wanted)
  -> Flag {
  using NETWORK::SESSIONS::State;
  for (Whole i = 0; i < TRIES; i += 1) {
    const State state = NETWORK::SESSIONS::settle(session);
    if (state == wanted) return true;
    if (state == State::CLOSED) return false;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  return false;
}

Flag dialed(const NETWORK::Socket &open, const NETWORK::Socket &shut) {
  using NETWORK::SESSIONS::State;
  const auto live = NETWORK::SESSIONS::create(open);
  const Flag answered =
    live != NETWORK::NONE && settled(live, State::OPEN) &&
    echoed(live, "over-dial");
  if (live != NETWORK::NONE) NETWORK::SESSIONS::destroy(live);
  const auto dead = NETWORK::SESSIONS::create(shut);
  const Flag refused =
    dead == NETWORK::NONE ||
    (!settled(dead, State::OPEN) &&
     NETWORK::SESSIONS::GET::state(dead) == State::CLOSED);
  return answered && refused;
}

int main(int count, char **arguments) {
  if (count < 4)
    return check("usage <socket path> <tcp port> <closed port>", false);
  const NETWORK::Endpoint local{arguments[1]};
  const NETWORK::Socket loop{
    "127.0.0.1", std::strtoul(arguments[2], nullptr, 10)};
  const NETWORK::Socket shut{
    "127.0.0.1", std::strtoul(arguments[3], nullptr, 10)};
  auto first = NETWORK::SESSIONS::create(local);
  auto second = NETWORK::SESSIONS::create(loop);
  Status status = check("unix", echoed(first, "over-unix"));
  status |= check("tcp", echoed(second, "over-tcp"));
  status |= check("dial", dialed(loop, shut));
  if (first != NETWORK::NONE) NETWORK::SESSIONS::destroy(first);
  if (second != NETWORK::NONE) NETWORK::SESSIONS::destroy(second);
  return status;
}
#elif SR_NETWORK_CARRIES(SR_UNIX) && SR_NETWORK_CARRIES(SR_TLS)
int main(int count, char **arguments) {
  if (count < 3) return check("usage <socket path> <tls port>", false);
  const NETWORK::Endpoint local{arguments[1]};
  const NETWORK::Tunnel secure{
    "127.0.0.1", std::strtoul(arguments[2], nullptr, 10)};
  auto first = NETWORK::SESSIONS::create(local);
  Status status = check("unix", echoed(first, "over-unix"));
  status |= check("tls", served(secure));
  if (first != NETWORK::NONE) NETWORK::SESSIONS::destroy(first);
  return status;
}
#else
int main() {
  std::cout << "streams: single-transport build" << std::endl;
  return 0;
}
#endif
