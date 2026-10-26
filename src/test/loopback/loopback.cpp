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
#include <chrono>
#include <iostream>
#include <network.hpp>
#include <thread>

namespace {
constexpr STRING::Hot PATH = "/x";
constexpr STRING::Hot WORD = "connected";
constexpr Whole PASSES = 20000;
constexpr Whole TRAILING = 200;
constexpr auto BEAT = std::chrono::milliseconds(1);
constexpr int PASSED = 0;
constexpr int FAILED = 1;
constexpr int USAGE = 2;

auto page(const String &) -> String { return WORD; }

auto pumped() -> Flag {
  for (Whole pass = 0; pass < PASSES; ++pass) {
    if (NETWORK::WEB::LOOPBACK::process()) return true;
    std::this_thread::sleep_for(BEAT);
  }
  return false;
}

void drain() {
  for (Whole pass = 0; pass < TRAILING; ++pass) {
    NETWORK::WEB::LOOPBACK::process();
    std::this_thread::sleep_for(BEAT);
  }
}

auto declared() -> int {
  NETWORK::WEB::LOOPBACK::adopt(true);
  const String address = NETWORK::WEB::LOOPBACK::open(PATH, &page);
  if (address.empty()) return FAILED;
  std::cout << "address " << address << std::endl;
  if (!pumped()) return FAILED;
  std::cout << "query " << NETWORK::WEB::LOOPBACK::GET::query() << std::endl;
  drain();
  NETWORK::WEB::LOOPBACK::close();
  return NETWORK::WEB::LOOPBACK::GET::query().empty() ? PASSED : FAILED;
}

auto undeclared() -> int {
  NETWORK::WEB::LOOPBACK::adopt(false);
  return NETWORK::WEB::LOOPBACK::open(PATH, &page).empty() ? PASSED : FAILED;
}

const Map<String, int (*)()> ARMS = {
  {"declared", &declared}, {"undeclared", &undeclared}};
}  // namespace

auto main(int count, char **arguments) -> int {
  const auto arm = count == 2 ? ARMS.find(arguments[1]) : ARMS.end();
  if (arm != ARMS.end()) return arm->second();
  std::cerr << "usage: loopback.out declared|undeclared\n";
  return USAGE;
}
