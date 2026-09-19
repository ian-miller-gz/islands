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
#include <cstring>
#include <iostream>
#include <thread>
#include <variant>

#include <network.hpp>
#include <topics.hpp>

namespace {

constexpr STRING::Hot SERVICE = "topics-wire";
constexpr int PASSES = 600;
constexpr auto DELAY = std::chrono::milliseconds(10);

#if SR_TOPICS == SR_LOCAL

auto broker() -> Status {
  auto here = NETWORK::listen(SERVICE, 8);
  if (here.handle == NETWORK::NONE) {
    std::cout << "broker: cannot listen" << std::endl;
    return 1;
  }
  TOPICS::serve(here.handle);
  for (;;) {
    TOPICS::relay();
    std::this_thread::sleep_for(DELAY);
  }
}

auto client() -> Status {
  auto reach = NETWORK::connect(SERVICE);
  if (reach.handle == NETWORK::NONE) {
    std::cout << "client: cannot connect" << std::endl;
    return 1;
  }
  auto bridge = TOPICS::link(reach.handle);
  auto down = TOPICS::subscribe("down");
  TOPICS::follow(bridge, "up");
  TOPICS::follow(bridge, "down");
  TOPICS::Message hello{{"n", Whole{42}}, {"who", String("client")}};
  for (int pass = 0, sent = 0; pass < PASSES; ++pass) {
    if (!sent++) TOPICS::publish("up", hello);
    TOPICS::relay();
    for (const auto &message : TOPICS::drain(down)) {
      const auto &held = message.at("n");
      Whole tag =
        std::holds_alternative<Whole>(held) ? std::get<Whole>(held) : 0;
      std::cout << "client: received down n=" << tag << std::endl;
      return 0;
    }
    std::this_thread::sleep_for(DELAY);
  }
  std::cout << "client: timed out" << std::endl;
  return 1;
}

#else

auto broker() -> Status {
  std::cout << "broker: unsupported (SR_TOPICS: SR_NONE)" << std::endl;
  return 2;
}
auto client() -> Status {
  std::cout << "client: unsupported (SR_TOPICS: SR_NONE)" << std::endl;
  return 2;
}

#endif

}  // namespace

auto wire(const char *mode) -> Status {
  if (std::strcmp(mode, "broker") == 0) return broker();
  if (std::strcmp(mode, "client") == 0) return client();
  std::cout << "unknown mode: " << mode << std::endl;
  return 2;
}
