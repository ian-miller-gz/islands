#pragma region COPYRIGHT
/* ===========================================================================
 *              (C) Copyright 2025 - Islands-Engine - Ian Miller             *
 =============================================================================
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU Affero General Public License as           *
 *  published by the Free Software Foundation, either version 3 of the       *
 *  License, or (at your option) any later version.                          *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Affero General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU Affero General Public License *
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 ============================================================================*/
#pragma endregion
#include <cartridge.hpp>
#include <island/input.hpp>
#include <island/overlay/windows.hpp>
#include <island/midi.hpp>
#include <island/gui/sac.hpp>
#include <island/window/backend/selection.hpp>
#include <island/window/backend.hpp>
#include <island/graphics/capture.hpp>
#include <island/graphics/vsync.hpp>
#include <island/graphics/windows.hpp>
#include <island/run.hpp>
#include <island/terminal/backend/listen.hpp>
#include <island/terminal/backend/repl.hpp>
#include <console.hpp>
#include <cstdlib>
#include <logger.hpp>
#include <metrics.hpp>
#include <network.hpp>
#include <sstream>
#include <string>

static constexpr STRING::Hot DIGITS = "0123456789";
static Flag pinned = false;
static Flag tapped = false;
static Flag webbed = false;
static Flag staged = false;
static Flag metered = false;
static Flag sized = false;
#if SR_LISTEN == SR_UNIX
static Flag listened = false;
#endif
static Vector<String> reach;

static constexpr STRING::Hot UNMAPPED = "ISLANDS_VISIBLE";

static void inherit() {
  const char *pin = std::getenv(UNMAPPED);
  if (pin && *pin) WINDOW::visible = String(pin) != "0";
}

static void visible(Flag on) {
  WINDOW::visible = on;
  COMMON::PLATFORM::assign(UNMAPPED, on ? "1" : "0");
}

static void window(const String &value) {
  auto split = value.find('=');
  if (split == String::npos) return;
  auto rest = value.substr(split + 1);
  if (rest.empty() || rest.find_first_not_of(DIGITS) != String::npos) return;
  OVERLAY::WINDOWS::preset(value.substr(0, split), std::stoi(rest) != 0);
}

static void webPin(const String &value) {
  if (value == "0" || value == "none" || value.empty()) return;
  std::istringstream entries(value);
  String entry;
  while (std::getline(entries, entry, ';')) {
    auto begin = entry.find_first_not_of(" \t");
    if (begin == String::npos) continue;
    reach.push_back(
      entry.substr(begin, entry.find_last_not_of(" \t") - begin + 1));
  }
}

static void apply(const String &flag, const String &value) {
  using namespace ISLAND::RUN;
  if (flag == "--frames") frames = std::stoul(value);
  if (flag == "--restarts") restarts = std::stoul(value);
  if (flag == "--width") {
    GFX::WINDOWS::MAIN::width = std::stoi(value);
    sized = true;
  }
  if (flag == "--height") {
    GFX::WINDOWS::MAIN::height = std::stoi(value);
    sized = true;
  }
  if (flag == "--capture") GFX::CAPTURE::path = value;
  if (flag == "--visible") ::visible(std::stoi(value) != 0);
  if (flag == "--vsync") GFX::VSYNC::on = std::stoi(value) != 0;
  if (flag == "--midi") MIDI::home(value);
  if (flag == "--overlay") {
    overlay = std::stoi(value) != 0;
    pinned = true;
  }
  if (flag == "--logs") {
    LOGGER::TAP::adopt(std::stoi(value) != 0);
    tapped = true;
  }
  if (flag == "--threads") {
    threads = std::stoi(value) != 0;
    staged = true;
  }
  if (flag == "--metrics") {
    METRICS::adopt(std::stoi(value) != 0);
    metered = true;
  }
  if (flag == "--web") {
    webPin(value);
    webbed = true;
  }
  if (flag == "--window") ::window(value);
  if (flag == "--log") LOGGER::FILTERS::add(value);
#if SR_CARTRIDGE == SR_NONE
  if (flag == "--cartridge") CARTRIDGE::path = value;
#endif
#if SR_LISTEN == SR_UNIX
  if (flag == "--listen") {
    TERMINAL::BACKEND::LISTEN::on = value != "0";
    if (value != "0" && value != "1")
      TERMINAL::BACKEND::LISTEN::self = NETWORK::Wire{value};
    listened = true;
  }
  if (flag == "--roster")
    TERMINAL::BACKEND::LISTEN::roster = NETWORK::Wire{value};
#endif
}

void ISLAND::RUN::parse(int count, char **values) {
  inherit();
  for (int i = 1; i + 1 < count; i += 2) apply(values[i], values[i + 1]);
}

static void splash() {
  if (!CARTRIDGE::splash) return;
  WINDOW::decorated = false;
  if (sized || (CARTRIDGE::width && CARTRIDGE::height)) return;
  GFX::WINDOWS::MAIN::width = WINDOW::BACKEND::FALLBACK::WIDTH;
  GFX::WINDOWS::MAIN::height = WINDOW::BACKEND::FALLBACK::HEIGHT;
}

void ISLAND::RUN::readopt() {
  if (webbed)
    NETWORK::WEB::adopt(::reach);
  else
    NETWORK::WEB::adopt(CARTRIDGE::web, CARTRIDGE::assets);
  NETWORK::WEB::SERVER::adopt(CARTRIDGE::server, CARTRIDGE::assets);
}

void ISLAND::RUN::adopt() {
  if (!pinned) overlay = CARTRIDGE::overlay;
  if (!tapped) LOGGER::TAP::adopt(CARTRIDGE::logs);
#if SR_LISTEN == SR_UNIX
  if (!listened)
    TERMINAL::BACKEND::LISTEN::on =
      CARTRIDGE::GET::entry() ? CARTRIDGE::listen : true;
#endif
  TERMINAL::BACKEND::REPL::on = !CARTRIDGE::fed;
  if (!staged) threads = CARTRIDGE::threads;
  latch = CARTRIDGE::latch;
  budget = CARTRIDGE::budget;
  if (!metered) METRICS::adopt(CARTRIDGE::metrics);
  WINDOW::decorated = CARTRIDGE::decorated;
  if (!sized && CARTRIDGE::width && CARTRIDGE::height) {
    GFX::WINDOWS::MAIN::width = static_cast<int>(CARTRIDGE::width);
    GFX::WINDOWS::MAIN::height = static_cast<int>(CARTRIDGE::height);
  }
  splash();
#if SR_WINDOW_BACKEND == SR_GLFW
  if (threads && overlay) {
    LOGGER::get("~/island/run::adopt")
      .warn(
        "Overlay on the GLFW window backend holds the frame stage on "
        "the main thread; running single-thread.");
    threads = false;
  }
#endif
  readopt();
  GUI::SAC::adopt(CARTRIDGE::components);
  LOGGER::get("~/island/run::adopt")
    .debug(threads ? "Stages (threads on)" : "Stages (threads off)");
}
