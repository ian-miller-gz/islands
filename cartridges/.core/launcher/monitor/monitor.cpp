// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cartridge/interface.hpp>
#include <logger.hpp>

#include "monitor.internal.hpp"
#define LOGGER_CATEGORY "~/cartridges/.core/launcher/monitor::monitor"

static constexpr Whole BACKLOG = 8;

static auto initialize(STRING::Hot) -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  const auto here = NETWORK::listen(MONITOR::NAME, BACKLOG);
  MONITOR::listener = here.handle;
  if (MONITOR::listener == NETWORK::NONE) {
    logger.error("Cannot listen on %s.", here.wire.c_str());
    return 1;
  }
  logger.info("Monitor listening on %s.", here.wire.c_str());
  return 0;
}

static void close() {
  for (auto &peer : MONITOR::peers)
    if (peer.session != NETWORK::NONE) NETWORK::SESSIONS::destroy(peer.session);
  MONITOR::peers.clear();
  MONITOR::roster.clear();
  if (MONITOR::listener == NETWORK::NONE) return;
  NETWORK::LISTENERS::destroy(MONITOR::listener);
  MONITOR::listener = NETWORK::NONE;
}

static const CARTRIDGE::Interface interface = {
  .version = CARTRIDGE::VERSION,
  .manifest = nullptr,
  .initialize = initialize,
  .serve = MONITOR::serve,
  .close = close};

extern "C" auto cartridge() -> const CARTRIDGE::Interface & {
  return interface;
}
