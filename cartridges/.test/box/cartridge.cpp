// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cartridge/interface.hpp>
#include "state.hpp"

constexpr auto SOURCE = "models/box.glb";

static auto initialize(STRING::Hot) -> Status {
  if (!GFX::MODEL::load(CARTRIDGE::GET::asset(SOURCE), BOX::scene))
    BOX::create();
  return 0;
}

static void close() {}

static const CARTRIDGE::Interface interface = {
  .version = CARTRIDGE::VERSION,
  .manifest = "cartridges/.test/box/manifest.yaml",
  .initialize = initialize,
  .frame = BOX::update,
  .close = close};

extern "C" auto cartridge() -> const CARTRIDGE::Interface& { return interface; }
