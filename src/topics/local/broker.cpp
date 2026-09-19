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
#include <utility>

#include <network.hpp>
#include <topics/local/local.internal.hpp>

namespace {

struct Following {
  Vector<String> topics;
};

using Peer = NETWORK::PEERS::Peer<Following>;

struct Hub {
  NETWORK::Handle listener = NETWORK::NONE;
  NETWORK::PEERS::Peers<Following> peers;
  Flag live = false;
};

Vector<Hub> hubs;

auto find(TOPICS::Broker handle) -> Hub * {
  if (handle.id == 0 || handle.id > hubs.size()) return nullptr;
  Hub &hub = hubs[handle.id - 1];
  return hub.live ? &hub : nullptr;
}

auto subscribed(const Peer &peer, const String &topic) -> Flag {
  for (const auto &followed : peer.state.topics)
    if (followed == topic) return true;
  return false;
}

void answer(Hub &hub, Peer &origin, const String &line) {
  auto frame = TOPICS::WIRE::decode(line);
  if (frame.verb == TOPICS::WIRE::Verb::SUBSCRIBE) {
    if (!subscribed(origin, frame.topic))
      origin.state.topics.push_back(frame.topic);
    return;
  }
  if (frame.verb != TOPICS::WIRE::Verb::PUBLISH) return;
  String out = line + "\n";
  for (auto &peer : hub.peers) {
    if (peer.session == NETWORK::NONE || peer.session == origin.session)
      continue;
    if (!subscribed(peer, frame.topic)) continue;
    if (!NETWORK::SESSIONS::push(peer.session, out)) peer.close();
  }
}

void service(Hub &hub) {
  hub.peers.accept(hub.listener);
  for (auto &peer : hub.peers) {
    Flag alive = peer.receive();
    for (String line; peer.next(line);) answer(hub, peer, line);
    if (!alive) peer.close();
  }
  hub.peers.reap();
}

void shutter(Hub &hub) {
  for (auto &peer : hub.peers) peer.close();
  hub.peers.reap();
  if (hub.listener != NETWORK::NONE) NETWORK::LISTENERS::destroy(hub.listener);
  hub = Hub{};
}

}  // namespace

namespace TOPICS {

auto serve(Whole listener) -> Broker {
  if (listener == NETWORK::NONE) return {};
  Hub fresh;
  fresh.listener = listener;
  fresh.live = true;
  for (Whole index = 0; index < hubs.size(); ++index)
    if (!hubs[index].live) {
      hubs[index] = std::move(fresh);
      return {index + 1};
    }
  hubs.push_back(std::move(fresh));
  return {static_cast<Whole>(hubs.size())};
}

void close(Broker handle) {
  if (Hub *hub = find(handle)) shutter(*hub);
}

}  // namespace TOPICS

void TOPICS::BROKER::pump() {
  for (Hub &hub : hubs)
    if (hub.live) service(hub);
}

void TOPICS::BROKER::purge() {
  for (Hub &hub : hubs)
    if (hub.live) shutter(hub);
  hubs.clear();
}
