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
namespace SOCKET = NETWORK::WEB::SOCKET;
constexpr STRING::Hot REACH = "http ws to 127.0.0.1";
constexpr STRING::Hot TRUST = "wss to localhost trusting ";
constexpr STRING::Hot ORIGIN = "ws://127.0.0.1:";
constexpr STRING::Hot SECURED = "wss://localhost:";
constexpr STRING::Hot ECHO = "/echo";
constexpr STRING::Hot SECRET = "secret";
constexpr Whole LONG = 70000;
constexpr Whole PASSES = 5000;
constexpr auto BEAT = std::chrono::milliseconds(1);
constexpr int PASSED = 0;
constexpr int FAILED = 1;
constexpr int USAGE = 2;
constexpr int ARGUMENTS = 4;

struct Exchange {
  STRING::Hot name;
  String sent;
  String answer;
};

const Exchange EXCHANGES[] = {
  {"echoed", "hello", "hello"},
  {"long", String(LONG, 'a'), String(LONG, 'a')},
  {"fragmented", "fragment", "fragmented"},
  {"pinged", "ping", "pong beat"}};

struct Refusal {
  STRING::Hot name;
  STRING::Hot origin;
  STRING::Hot path;
  STRING::Hot reason;
};

const Refusal REFUSALS[] = {
  {"undeclared", "ws://localhost:", "/?secret=word", ""},
  {"forged", ORIGIN, "/forged?secret=word", "socket refused: accept"},
  {"unspoken", "http://127.0.0.1:", "/", "socket refused: scheme http"}};

auto awaited(SOCKET::Socket socket) -> String {
  for (Whole pass = 0; pass < PASSES && SOCKET::connected(socket); ++pass) {
    const Vector<String> messages = SOCKET::receive(socket);
    if (!messages.empty()) return messages.front();
    std::this_thread::sleep_for(BEAT);
  }
  return {};
}

auto refused(const Refusal &refusal, const String &port) -> Flag {
  String error;
  const String url = refusal.origin + port + refusal.path;
  return SOCKET::open(url, error) == SOCKET::NONE && !error.empty() &&
         !error.contains(SECRET) &&
         (!*refusal.reason || error == refusal.reason);
}

auto spoken(SOCKET::Socket socket, const Exchange &exchange) -> Flag {
  return SOCKET::send(socket, exchange.sent) &&
         awaited(socket) == exchange.answer;
}

auto closed(SOCKET::Socket socket) -> Flag {
  return SOCKET::send(socket, "close") && awaited(socket).empty() &&
         !SOCKET::connected(socket) && !SOCKET::send(socket, "late");
}

auto secured(const String &port) -> Flag {
  String error;
  const auto socket = SOCKET::open(SECURED + port + ECHO, error);
  return spoken(socket, EXCHANGES[1]) && closed(socket);
}

void report(STRING::Hot name, Flag good, int &verdict) {
  std::cout << (good ? "PASS " : "FAIL ") << name << std::endl;
  if (!good) verdict = FAILED;
}
}  // namespace

auto main(int count, char **arguments) -> int {
  if (count != ARGUMENTS) return USAGE;
  NETWORK::WEB::adopt({REACH, TRUST + String(arguments[3])});
  String error;
  const auto socket = SOCKET::open(ORIGIN + String(arguments[1]) + ECHO, error);
  int verdict = PASSED;
  report("open", socket != SOCKET::NONE, verdict);
  for (const Exchange &exchange : EXCHANGES)
    report(exchange.name, spoken(socket, exchange), verdict);
  for (const Refusal &refusal : REFUSALS)
    report(refusal.name, refused(refusal, arguments[1]), verdict);
  report("closed", closed(socket), verdict);
  report("secured", secured(arguments[2]), verdict);
  return verdict;
}
