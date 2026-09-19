// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cartridge/interface.hpp>
#include <logger.hpp>

#include "roster.internal.hpp"
#define LOGGER_CATEGORY "~/cartridges/.test/roster::roster"

static constexpr Whole BACKLOG = 8;

static auto initialize(STRING::Hot) -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  const auto here = NETWORK::listen(ROSTER::NAME, BACKLOG);
  ROSTER::listener = here.handle;
  if (ROSTER::listener == NETWORK::NONE) {
    logger.error("Cannot listen on %s.", here.wire.c_str());
    return 1;
  }
  logger.info("Roster listening on %s.", here.wire.c_str());
  return 0;
}

static void close() {
  for (auto &peer : ROSTER::peers) peer.close();
  ROSTER::peers.reap();
  if (ROSTER::listener == NETWORK::NONE) return;
  NETWORK::LISTENERS::destroy(ROSTER::listener);
  ROSTER::listener = NETWORK::NONE;
}

static const CARTRIDGE::Interface interface = {
  .version = CARTRIDGE::VERSION,
  .manifest = nullptr,
  .initialize = initialize,
  .serve = ROSTER::serve,
  .close = close};

extern "C" auto cartridge() -> const CARTRIDGE::Interface & {
  return interface;
}
