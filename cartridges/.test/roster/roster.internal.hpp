// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "roster.hpp"

namespace ROSTER {

struct Entry {
  String line;
};

using Peer = NETWORK::PEERS::Peer<Entry>;

inline NETWORK::Handle listener = NETWORK::NONE;
inline NETWORK::PEERS::Peers<Entry> peers;

void serve();

}  // namespace ROSTER
