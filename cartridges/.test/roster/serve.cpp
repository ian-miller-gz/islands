// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <logger.hpp>

#include "roster.internal.hpp"
#define LOGGER_CATEGORY "~/cartridges/.test/roster::serve"

static auto list() -> String {
  String lines;
  for (const auto &peer : ROSTER::peers)
    if (!peer.state.line.empty()) lines += peer.state.line + "\n";
  return lines;
}

static auto parse(const String &line) -> String {
  String prefix = String(ROSTER::HELLO) + ROSTER::TAB;
  if (line.rfind(prefix, 0) != 0) return "";
  Whole count = std::count(line.begin(), line.end(), ROSTER::TAB);
  if (count != ROSTER::FIELDS) return "";
  return line.substr(prefix.size());
}

static auto answer(ROSTER::Peer &peer, const String &line) -> Flag {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  auto entry = parse(line);
  if (!entry.empty()) {
    peer.state.line = entry;
    logger.debug("Registered: %s", entry.c_str());
    return true;
  }
  if (line == ROSTER::LIST) {
    NETWORK::SESSIONS::send(peer.session, list());
    return false;
  }
  return true;
}

void ROSTER::serve() {
  peers.accept(listener);
  for (auto &peer : peers) {
    Flag alive = peer.receive();
    for (String line; peer.next(line);)
      if (!answer(peer, line)) {
        peer.close();
        break;
      }
    if (!alive) peer.close();
  }
  peers.reap();
}
