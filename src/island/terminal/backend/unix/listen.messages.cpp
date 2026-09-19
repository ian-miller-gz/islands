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
#include <island/run.hpp>
#include <island/terminal/backend/unix/listen.internal.hpp>
#include <format>
#include <logger.hpp>
#include <messages/messages.hpp>
#include <metrics.hpp>
#include <string>
#define LOGGER_CATEGORY "~/island/terminal::listen"

#if SR_LISTEN == SR_UNIX

static constexpr STRING::Hot TOPIC = "frames";

static auto step() -> String {
  static auto previous = std::chrono::steady_clock::now();
  auto now = std::chrono::steady_clock::now();
  auto lapse = std::chrono::duration<Float, std::milli>(now - previous);
  previous = now;
  return std::format("{:.3f}", lapse.count());
}

void TERMINAL::BACKEND::LISTEN::join() {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  if (broker != NETWORK::NONE) return;
  const auto reached = NETWORK::connect(MESSAGES::NAME);
  broker = reached.handle;
  if (broker == NETWORK::NONE)
    return logger.debug("No message broker at %s.", reached.wire.c_str());
  logger.debug("Publishing to the broker at %s.", reached.wire.c_str());
}

static auto send(const String &topic, const String &payload) -> Flag {
  using namespace TERMINAL::BACKEND::LISTEN;
  return NETWORK::SESSIONS::push(broker, MESSAGES::publish(topic, payload));
}

void TERMINAL::BACKEND::LISTEN::publish() {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  if (broker == NETWORK::NONE) return;
  auto count = std::to_string(ISLAND::RUN::GET::frame());
  Flag alive = ::send(TOPIC, count + MESSAGES::TAB + step());
  for (const auto &name : METRICS::GET::names()) {
    if (!alive) break;
    alive =
      ::send(name, std::format("{:.3f}", METRICS::GET::last(name.c_str())));
  }
  if (alive) return;
  leave();
  logger.debug("Message broker gone; publishing stopped.");
}

void TERMINAL::BACKEND::LISTEN::leave() {
  if (broker == NETWORK::NONE) return;
  NETWORK::SESSIONS::destroy(broker);
  broker = NETWORK::NONE;
}

#endif
