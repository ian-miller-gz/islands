// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cartridge.hpp>
#include <cartridge/load.internal.hpp>
#include <logger.hpp>
#if SR_CARTRIDGE != SR_NONE

// The proof host's static-delivery arm, the reef's refusal shape: the
// statics: fold targets the project binary (the first builds: entry), so
// a static build folds no cartridge into jetty — this arm satisfies the
// loader's delivery seam without referencing the linked entry symbol and
// refuses at run time. The dynamic loader is what this host exists to
// prove, so its gold rightly skips on static cells; this arm only keeps
// the target linkable there. Every host that links src/cartridge owes
// the seam one delivery arm — the island folds, the reef and jetty
// refuse.

auto CARTRIDGE::bind(LOGGER::Category &logger) -> Status {
  logger.error("Static delivery does not reach this host.");
  return 1;
}

void CARTRIDGE::release() {}

auto CARTRIDGE::configured() -> Flag { return false; }

auto CARTRIDGE::GET::bundle() -> String { return NONE; }

#endif
