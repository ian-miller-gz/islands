// SPDX-License-Identifier: AGPL-3.0-or-later
#include <filesystem>

#include "state.hpp"

namespace fs = std::filesystem;

void LAUNCHER::STORE::remove(Whole row) {
  if (installing != ISLANDS::SELECT::NONE) return;
  if (row >= offers.size() || !offers[row].installed) return;
  std::error_code slip;
  fs::remove_all(fs::path(GET::home()) / offers[row].leaf, slip);
  offers[row].state.clear();
  installed(offers[row]);
  LAUNCHER::scan();
}

void LAUNCHER::STORE::call(Whole row) {
  if (row < offers.size() && offers[row].installed) return remove(row);
  install(row);
}

auto LAUNCHER::STORE::GET::progress() -> Float {
  if (installing == ISLANDS::SELECT::NONE) return 0.0f;
  const Vector<Step> &walk = steps();
  for (Whole at = 0; at < walk.size(); at += 1)
    if (offers[installing].state == walk[at].word)
      return static_cast<Float>(at) / static_cast<Float>(walk.size());
  return 0.0f;
}
