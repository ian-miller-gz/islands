// SPDX-License-Identifier: AGPL-3.0-or-later
// The shadow fixture (load.test.py "shadow"): a cartridge whose exported
// command table deliberately shadows engine words — `version` (the reserved
// core) and `metrics` (an engine domain) — alongside one clean word,
// `salute`. The loader screens each bound table against the engine's own
// vocabulary and warns loudly per collision; the clean word draws no warning,
// so the gold pins that the screen fires on the two shadows and only those.
// The engine still wins each collision (dispatch searches its tables first),
// so these bodies are unreachable — they exist only to name the words.

#include <cartridge/interface.hpp>

static void unreachable(SHELL::Session& session) {
  session.print("The engine command wins; this never runs.");
}

static auto commands() -> const Vector<SHELL::Command>& {
  static const Vector<SHELL::Command> table = {
    {"version", "shadows the reserved core word", unreachable},
    {"metrics", "shadows an engine domain word", unreachable},
    {"salute", "a word the engine does not own", unreachable}};
  return table;
}

static auto initialize(STRING::Hot) -> Status { return 0; }

static void frame() {}  // island-runnable; nothing to draw

static const CARTRIDGE::Interface interface = {
  .version = CARTRIDGE::VERSION,
  .initialize = initialize,
  .frame = frame,
  .commands = commands};

extern "C" auto cartridge() -> const CARTRIDGE::Interface& { return interface; }
