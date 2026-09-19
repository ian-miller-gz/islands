// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <sys/types.h>

#include <network.hpp>

#include "monitor.hpp"

namespace MONITOR {

struct Entry {
  pid_t pid = 0;
  String bundle;
  Flag running = true;
};

struct Peer {
  NETWORK::Handle session = NETWORK::NONE;
  String buffer;
};

inline NETWORK::Handle listener = NETWORK::NONE;
inline Vector<Peer> peers;
inline Vector<Entry> roster;

void serve();
void dispatch(const String &line);
void sweep();
auto snapshot() -> String;

}  // namespace MONITOR
