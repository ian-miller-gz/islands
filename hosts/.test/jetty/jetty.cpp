// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cartridge.hpp>
#include <common.hpp>
#include <console.hpp>
#include <logger.hpp>
#include <metrics.hpp>
#include <network.hpp>

#include <csignal>
#include <iostream>

// The engine's proof host: a resident main composing the
// headless subset — the cartridge seam, console instances, the engine
// shell's dispatch, and the network — per hosts/'s contract.
// The gold driver (tests/jetty.py) scripts a run over a serving bundle;
// the `jetty` proof lines below are what it pins. Run from the repo
// root: this host never chdirs, so bundle paths resolve from the cwd.

static volatile std::sig_atomic_t running = 1;
static void halt(int) { running = 0; }

// Block the shutdown signals before any wait: NETWORK::wait unblocks
// them atomically inside its ppoll (the worked-loop discipline).
static void arm() {
  sigset_t held;
  sigemptyset(&held);
  sigaddset(&held, SIGINT);
  sigaddset(&held, SIGTERM);
  sigprocmask(SIG_BLOCK, &held, nullptr);
  std::signal(SIGINT, halt);
  std::signal(SIGTERM, halt);
}

// The console proof: seed the engine console and submit one line — the
// dispatch seam (SHELL::ENGINE::dispatch) resolves headless because this
// host links src/shell/engine, no island terminal anywhere. The echoed
// answer lands in the instance's scrollback; printing it is the pin.
static void proof() {
  auto &console = CONSOLES::GET::console();
  console.line = "echo ahoy";
  CONSOLES::submit(console);
  std::cout << "jetty console: " << console.lines.back() << std::endl;
}

// Deliver the cartridge per the hosting checklist: validate the one hook
// this host drives (refuse loudly naming it — the reef's shape), adopt
// the manifest's runtime preferences between load and initialize.
static auto deliver() -> Status {
  if (CARTRIDGE::load()) return 1;
  const auto *bound = CARTRIDGE::GET::entry();
  if (!bound || !bound->serve) {
    std::cout << "jetty refuses: no serve hook" << std::endl;
    return 1;
  }
  LOGGER::TAP::adopt(CARTRIDGE::logs);
  NETWORK::WEB::adopt(CARTRIDGE::web);
  METRICS::adopt(CARTRIDGE::metrics);
  REQUIREMENTS::initialize();
  CONSOLES::initialize();  // content services before content (promise 2)
  CARTRIDGE::initialize();
  return 0;
}

static auto run() -> Status {
  if (deliver()) return 1;
  proof();
  arm();
  while (running && !CARTRIDGE::finished) {
    NETWORK::wait();
    CARTRIDGE::serve();
  }
  CARTRIDGE::close();     // teardown inverts (promise 6): the cartridge,
  CONSOLES::close();      // the surfaces that could hold its state,
  REQUIREMENTS::close();  // its dependencies,
  CARTRIDGE::unload();    // the .so last
  std::cout << "jetty teardown: complete" << std::endl;
  return CARTRIDGE::status;
}

int main(int argc, char **argv) {
  LOGGER::initialize();
#if SR_CARTRIDGE == SR_NONE  // the path is the dynamic loader's state; a
  // static build compiles the selection out (the reef's parse shape) and
  // this host's delivery arm refuses inside load()
  if (argc > 2 && String(argv[1]) == "--cartridge") CARTRIDGE::path = argv[2];
#endif
  try {
    return run();
  } catch (const std::exception &error) {
    std::cout << error.what() << std::endl;  // what throws ends here
    return 1;
  }
}
