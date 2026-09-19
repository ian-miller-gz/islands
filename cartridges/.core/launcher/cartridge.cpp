// SPDX-License-Identifier: AGPL-3.0-or-later
#include "state.hpp"

static auto initialize(STRING::Hot assets) -> Status {
  LAUNCHER::document = GUI::mount(assets, LAUNCHER::LAYOUT);
  if (LAUNCHER::document == GUI::NONE) return 1;
  LAUNCHER::IDLE::create();
  LAUNCHER::scan();
  LAUNCHER::WATCH::attach();
  return 0;
}

static void close() {
  LAUNCHER::stop();
  LAUNCHER::WATCH::detach();
}

static const CARTRIDGE::Interface interface = {
  .version = CARTRIDGE::VERSION,
  .manifest = "cartridges/.core/launcher/manifest.yaml",
  .initialize = initialize,
  .frame = LAUNCHER::update,
  .close = close};

extern "C" auto cartridge() -> const CARTRIDGE::Interface& { return interface; }
