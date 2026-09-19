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
using NETWORK::WEB::SERVER::Request;
using NETWORK::WEB::SERVER::Response;

constexpr STRING::Hot TEXT = "text/plain; charset=utf-8";
constexpr Whole PASSES = 20000;
constexpr Whole TRAILING = 200;
constexpr auto BEAT = std::chrono::milliseconds(1);
Flag finished = false;

auto page(const Request &ask) -> Response {
  if (ask.path == "/done") {
    finished = true;
    return {200, TEXT, "closing\n"};
  }
  if (ask.path == "/revoke") {
    NETWORK::WEB::SERVER::revoke("hotel");
    return {200, TEXT, "revoked hotel\n"};
  }
  if (ask.path == "/where")
    return {200, TEXT, "path=" + ask.path + " query=" + ask.query + "\n"};
  if (ask.path == "/") return {200, TEXT, "reader: " + ask.reader + "\n"};
  return {404, TEXT, "no such page: " + ask.path + "\n"};
}

auto stated(const String &wire) -> Vector<String> {
  return {wire + " as reader.crt reader.key"};
}
}  // namespace

auto main(int count, char **arguments) -> int {
  if (count != 4) {
    std::cerr << "usage: reader.out serve|dark <address:port> <home>\n";
    return 2;
  }
  const String arm = arguments[1];
  NETWORK::WEB::SERVER::adopt(stated(arguments[2]), arguments[3]);
  NETWORK::WEB::SERVER::grant("wife", "kitchen");
  NETWORK::WEB::SERVER::grant("hotel", "bar");
  const Flag opened = NETWORK::WEB::SERVER::open(&page);
  if (arm == "dark") return opened ? 1 : 0;
  if (!opened) return 1;
  std::cout << "serving " << NETWORK::WEB::SERVER::GET::wire() << std::endl;
  Whole pass = 0;
  for (; pass < PASSES && !finished; ++pass) {
    NETWORK::WEB::SERVER::process();
    std::this_thread::sleep_for(BEAT);
  }
  for (Whole trailing = 0; trailing < TRAILING; ++trailing) {
    NETWORK::WEB::SERVER::process();
    std::this_thread::sleep_for(BEAT);
  }
  NETWORK::WEB::SERVER::close();
  return NETWORK::WEB::SERVER::GET::serving() || pass == PASSES ? 1 : 0;
}
