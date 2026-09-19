// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

namespace LAUNCHER::WATCH {
constexpr STRING::Hot BUNDLE = "cartridges/.core/launcher/monitor";

inline NETWORK::Handle session = NETWORK::NONE;
inline String buffer;
inline Flag ready = false;
inline Whole tracked = 0;
inline Float asked = 0;

void attach();
void poll();
void detach();
void announce(const LAUNCHER::Child &child);
void release(pid_t pid);
}  // namespace LAUNCHER::WATCH
