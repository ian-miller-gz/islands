// SPDX-License-Identifier: AGPL-3.0-or-later

#include <cartridge/interface.hpp>
#include <fstream>
#include <iostream>

static auto initialize(STRING::Hot assets) -> Status {
  std::cout << "Demo initialized." << std::endl;
  std::ifstream file(String(assets) + "message.txt");
  String message;
  std::getline(file, message);
  std::cout << "Demo says: " << message << std::endl;
  return 0;
}

static void frame() {
  static Flag announced = false;
  if (!announced) std::cout << "Demo running." << std::endl;
  announced = true;
}

static void close() { std::cout << "Demo closed." << std::endl; }

static const CARTRIDGE::Interface interface = {
  .version = CARTRIDGE::VERSION,
  .manifest = nullptr,
  .initialize = initialize,
  .frame = frame,
  .close = close};

extern "C" auto cartridge() -> const CARTRIDGE::Interface& { return interface; }
