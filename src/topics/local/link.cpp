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

struct Followed {
  String topic;
  TOPICS::Subscription forward;
};

struct Bridge {
  NETWORK::Handle session = NETWORK::NONE;
  NETWORK::LINES::Framer lines;
  Vector<Followed> topics;
  Flag live = false;
};

Vector<Bridge> bridges;

auto find(TOPICS::Link handle) -> Bridge * {
  if (handle.id == 0 || handle.id > bridges.size()) return nullptr;
  Bridge &bridge = bridges[handle.id - 1];
  return bridge.live ? &bridge : nullptr;
}

auto followed(const Bridge &bridge, const String &topic) -> Flag {
  for (const auto &entry : bridge.topics)
    if (entry.topic == topic) return true;
  return false;
}

void retire(Bridge &bridge) {
  for (auto &entry : bridge.topics) TOPICS::unsubscribe(entry.forward);
  if (bridge.session != NETWORK::NONE)
    NETWORK::SESSIONS::destroy(bridge.session);
  bridge = Bridge{};
}

void service(Bridge &bridge) {
  for (auto &entry : bridge.topics)
    for (const auto &message : TOPICS::drain(entry.forward))
      if (!NETWORK::SESSIONS::push(
            bridge.session, TOPICS::WIRE::publish(entry.topic, message)))
        return retire(bridge);
  String bytes;
  Flag alive = NETWORK::SESSIONS::receive(bridge.session, bytes);
  bridge.lines.feed(bytes);
  for (String line; bridge.lines.next(line);) {
    auto frame = TOPICS::WIRE::decode(line);
    if (
      frame.verb == TOPICS::WIRE::Verb::PUBLISH &&
      followed(bridge, frame.topic))
      TOPICS::publish(frame.topic.c_str(), frame.message);
  }
  for (auto &entry : bridge.topics) TOPICS::drain(entry.forward);
  if (!alive) retire(bridge);
}

}  // namespace

namespace TOPICS {

auto link(Whole session) -> Link {
  if (session == NETWORK::NONE) return {};
  Bridge fresh;
  fresh.session = session;
  fresh.live = true;
  for (Whole index = 0; index < bridges.size(); ++index)
    if (!bridges[index].live) {
      bridges[index] = std::move(fresh);
      return {index + 1};
    }
  bridges.push_back(std::move(fresh));
  return {static_cast<Whole>(bridges.size())};
}

void follow(Link handle, STRING::Hot topic) {
  Bridge *bridge = find(handle);
  if (!bridge || followed(*bridge, topic)) return;
  bridge->topics.push_back({topic, subscribe(topic)});
  NETWORK::SESSIONS::push(bridge->session, WIRE::subscribe(topic));
}

void unlink(Link handle) {
  if (Bridge *bridge = find(handle)) retire(*bridge);
}

void relay() {
  LINK::pump();
  BROKER::pump();
}

}  // namespace TOPICS

void TOPICS::LINK::pump() {
  for (Bridge &bridge : bridges)
    if (bridge.live) service(bridge);
}

void TOPICS::LINK::purge() {
  for (Bridge &bridge : bridges)
    if (bridge.live) retire(bridge);
  bridges.clear();
}
