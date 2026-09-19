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
#include <logger.hpp>
#include <network/backend/posix/sessions.internal.hpp>
#define LOGGER_CATEGORY "~/network::sessions"

static constexpr Whole CHUNK = 512;
static constexpr Integer PATIENCE = 1000;

using NETWORK::SESSIONS::descriptors;
using NETWORK::SESSIONS::fetch;

static Flag writable(NETWORK::Descriptor descriptor) {
  pollfd query{.fd = descriptor, .events = POLLOUT, .revents = 0};
  return NETWORK::poll(&query, 1, PATIENCE) > 0;
}

auto NETWORK::SESSIONS::adopt(Descriptor descriptor, const Codec *codec)
  -> Handle {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  logger.debug("Session opened.");
  for (Whole slot = 0; slot < descriptors.size(); ++slot)
    if (descriptors[slot] == CLOSED) {
      descriptors[slot] = descriptor;
      codecs[slot] = codec;
      return slot;
    }
  descriptors.push_back(descriptor);
  codecs.push_back(codec);
  return descriptors.size() - 1;
}

auto NETWORK::SESSIONS::receive(Handle session, String &data) -> Flag {
  Descriptor descriptor = fetch(session);
  if (descriptor == CLOSED) return false;
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

void NETWORK::SESSIONS::destroy(Handle session) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  if (!connected(session)) return;
  if (const Codec *codec = codecs[session]) codec->close(descriptors[session]);
  close(descriptors[session]);
  descriptors[session] = CLOSED;
  codecs[session] = nullptr;
  logger.debug("Session closed.");
}
