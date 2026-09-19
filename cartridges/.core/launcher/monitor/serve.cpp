// SPDX-License-Identifier: AGPL-3.0-or-later
#include "monitor.internal.hpp"

static auto drain(MONITOR::Peer &peer) -> Flag {
  if (peer.session == NETWORK::NONE) return false;
  Flag alive = NETWORK::SESSIONS::receive(peer.session, peer.buffer);
  for (auto cut = peer.buffer.find('\n'); cut != String::npos;
       cut = peer.buffer.find('\n')) {
    String line = peer.buffer.substr(0, cut);
    peer.buffer.erase(0, cut + 1);
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (!line.empty()) MONITOR::dispatch(line);
  }
  return alive && peer.session != NETWORK::NONE;
}

static void answer(const String &block) {
  for (auto &peer : MONITOR::peers) {
    if (peer.session == NETWORK::NONE) continue;
    if (NETWORK::SESSIONS::push(peer.session, block)) continue;
    NETWORK::SESSIONS::destroy(peer.session);
    peer.session = NETWORK::NONE;
  }
}

void MONITOR::serve() {
  for (auto session = NETWORK::LISTENERS::accept(listener);
       session != NETWORK::NONE; session = NETWORK::LISTENERS::accept(listener))
    peers.push_back({session});
  for (Whole at = 0; at < peers.size();)
    if (drain(peers[at])) {
      ++at;
    } else {
      if (peers[at].session != NETWORK::NONE)
        NETWORK::SESSIONS::destroy(peers[at].session);
      peers.erase(peers.begin() + at);
    }
  sweep();
  answer(snapshot());
}
