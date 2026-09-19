// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cartridge/interface.hpp>
#include <logger.hpp>

#include "messages.internal.hpp"
#define LOGGER_CATEGORY "~/cartridges/.test/messages::messages"

static constexpr Whole BACKLOG = 8;

static auto initialize(STRING::Hot) -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  const auto here = NETWORK::listen(MESSAGES::NAME, BACKLOG);
  MESSAGES::listener = here.handle;
  if (MESSAGES::listener == NETWORK::NONE) {
    logger.error("Cannot listen on %s.", here.wire.c_str());
    return 1;
  }
  logger.info("Broker listening on %s.", here.wire.c_str());
  return 0;
}

static void close() {
  for (auto &peer : MESSAGES::peers) peer.close();
  MESSAGES::peers.reap();
  if (MESSAGES::listener == NETWORK::NONE) return;
  NETWORK::LISTENERS::destroy(MESSAGES::listener);
  MESSAGES::listener = NETWORK::NONE;
}

static const CARTRIDGE::Interface interface = {
  .version = CARTRIDGE::VERSION,
  .manifest = nullptr,
  .initialize = initialize,
  .serve = MESSAGES::serve,
  .close = close};

extern "C" auto cartridge() -> const CARTRIDGE::Interface & {
  return interface;
}
