// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "messages.hpp"

namespace MESSAGES {

struct Following {
  Vector<String> topics;
};

using Peer = NETWORK::PEERS::Peer<Following>;

inline NETWORK::Handle listener = NETWORK::NONE;
inline NETWORK::PEERS::Peers<Following> peers;

void serve();

}  // namespace MESSAGES
