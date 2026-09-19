// SPDX-License-Identifier: AGPL-3.0-or-later
#include <logger.hpp>

#include "messages.internal.hpp"
#define LOGGER_CATEGORY "~/cartridges/.test/messages::serve"

static auto subscribed(const MESSAGES::Peer &peer, const String &topic)
  -> Flag {
  for (const auto &followed : peer.state.topics)
    if (followed == topic) return true;
  return false;
}

static void relay(const String &topic, const String &payload) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  auto line = MESSAGES::deliver(topic, payload);
  for (auto &peer : MESSAGES::peers) {
    if (peer.session == NETWORK::NONE || !subscribed(peer, topic)) continue;
    if (NETWORK::SESSIONS::push(peer.session, line)) continue;
    peer.close();
    logger.debug("Dropped a subscriber of %s.", topic.c_str());
  }
}

static void answer(MESSAGES::Peer &peer, const String &line) {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  auto cut = line.find(MESSAGES::TAB);
  if (cut == String::npos) return;
  auto verb = line.substr(0, cut);
  auto rest = line.substr(cut + 1);
  if (verb == MESSAGES::SUBSCRIBE && !rest.empty()) {
    if (!subscribed(peer, rest)) peer.state.topics.push_back(rest);
    return logger.debug("Subscribed to %s.", rest.c_str());
  }
  if (verb != MESSAGES::PUBLISH) return;
  auto split = rest.find(MESSAGES::TAB);
  if (split == String::npos) return;
  relay(rest.substr(0, split), rest.substr(split + 1));
}

void MESSAGES::serve() {
  peers.accept(listener);
  for (auto &peer : peers) {
    Flag alive = peer.receive();
    for (String line; peer.next(line);) answer(peer, line);
    if (!alive) peer.close();
  }
  peers.reap();
}
