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
#include <logger.hpp>
#include <network/sockets/dial.internal.hpp>
#include <network/sockets/sessions.internal.hpp>
#define LOGGER_CATEGORY "~/network::sessions"

static constexpr Whole CHUNK = 512;

using NETWORK::SESSIONS::descriptors;
using NETWORK::SESSIONS::fetch;

static Flag writable(NETWORK::Descriptor descriptor) {
  pollfd query{.fd = descriptor, .events = POLLOUT, .revents = 0};
  return NETWORK::poll(&query, 1, NETWORK::SESSIONS::PATIENCE) > 0;
}

static auto stamp() -> int64_t {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
           std::chrono::steady_clock::now().time_since_epoch())
    .count();
}

// Whether a dialing session must still be resolved before use: a glance at
// the connect, which opens the session, keeps it dialing, or closes it.
static auto unsettled(NETWORK::Handle session) -> Flag {
  using NETWORK::SESSIONS::State;
  if (!NETWORK::SESSIONS::dialing[session]) return false;
  return NETWORK::SESSIONS::settle(session) != State::OPEN;
}

auto NETWORK::SESSIONS::adopt(
  Descriptor descriptor, const Codec *codec, Flag pending) -> Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  logger.debug(pending ? "Session dialing." : "Session opened.");
  for (Whole slot = 0; slot < descriptors.size(); ++slot)
    if (descriptors[slot] == CLOSED) {
      descriptors[slot] = descriptor;
      codecs[slot] = codec;
      dialing[slot] = pending;
      since[slot] = stamp();
      return slot;
    }
  descriptors.push_back(descriptor);
  codecs.push_back(codec);
  dialing.push_back(pending);
  since.push_back(stamp());
  return descriptors.size() - 1;
}

auto NETWORK::SESSIONS::receive(Handle session, String &data) -> Flag {
  Descriptor descriptor = fetch(session);
  if (descriptor == CLOSED) return false;
  if (unsettled(session)) return connected(session);
  if (const Codec *codec = codecs[session])
    return codec->receive(descriptor, data);
  char chunk[CHUNK];
  while (true) {
    auto received = ::recv(descriptor, chunk, sizeof(chunk), 0);
    if (received > 0) {
      data.append(chunk, received);
      continue;
    }
    if (received == 0) return false;
    if (failing() == INTERRUPTED) continue;
    return failing() == AGAIN;
  }
}

auto NETWORK::SESSIONS::send(Handle session, const String &data) -> Flag {
  Descriptor descriptor = fetch(session);
  if (descriptor == CLOSED) return false;
  if (unsettled(session)) return false;
  if (const Codec *codec = codecs[session])
    return codec->send(descriptor, data);
  for (Whole sent = 0; sent < data.size();) {
    auto wrote =
      ::send(descriptor, data.data() + sent, data.size() - sent, QUIET);
    if (wrote > 0) {
      sent += wrote;
      continue;
    }
    if (failing() == INTERRUPTED) continue;
    if (failing() == AGAIN && writable(descriptor)) continue;
    return false;
  }
  return true;
}

auto NETWORK::SESSIONS::settle(Handle session, Integer patience) -> State {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  if (!connected(session)) return State::CLOSED;
  if (!dialing[session]) return State::OPEN;
  const Dial verdict = resolved(descriptors[session], patience);
  if (verdict == Dial::OPEN) {
    dialing[session] = false;
    logger.debug("Session opened.");
    return State::OPEN;
  }
  if (verdict == Dial::PENDING && stamp() - since[session] < PATIENCE)
    return State::DIALING;
  logger.debug(
    verdict == Dial::PENDING ? "Dial unanswered; giving up."
                             : "Dial refused.");
  destroy(session);
  return State::CLOSED;
}

auto NETWORK::SESSIONS::GET::state(Handle session) -> State {
  if (!connected(session)) return State::CLOSED;
  return dialing[session] ? State::DIALING : State::OPEN;
}

void NETWORK::SESSIONS::destroy(Handle session) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  if (!connected(session)) return;
  if (const Codec *codec = codecs[session]) codec->close(descriptors[session]);
  close(descriptors[session]);
  descriptors[session] = CLOSED;
  codecs[session] = nullptr;
  dialing[session] = false;
  logger.debug("Session closed.");
}
