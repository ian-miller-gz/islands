// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

namespace LAUNCHER::IDLE {

struct Field {
  Float time = 0;
  Float aspect = 1;
  Float pad0 = 0;
  Float pad1 = 0;
};

inline GFX::Handle pipeline = GFX::NONE;
inline GFX::Handle night = GFX::NONE;
inline GFX::Handle quad = GFX::NONE;

void create();
void paint(GFX::Pass &pass);
}  // namespace LAUNCHER::IDLE
