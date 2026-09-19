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
#include <island/terminal/backend/repl.internal.hpp>
#include <island/terminal/backend/repl.hpp>
#include <island/terminal/clear.hpp>
#include <shell/engine/engine.hpp>
#include <iostream>
#include <logger.hpp>
#define LOGGER_CATEGORY "~/island/terminal::repl"

static Flag active = false;
static Flag tty = false;

static constexpr STRING::Hot PROMPT = "> ";

namespace {
struct Stdio final : SHELL::Session {
  void print(const String &line) override { std::cout << line << "\n"; }
  void clear() override {
    if (tty) std::cout << TERMINAL::CLEAR;
  }
  void close() override { active = false; }
};
}  // namespace

static void respond(const String &line) {
  Stdio session;
  SHELL::ENGINE::dispatch(session, line);
}

void TERMINAL::BACKEND::REPL::initialize() {
  active = on;
  if (!active) return;
  tty = attached();
  LOGGER::get(LOGGER_CATEGORY).debug("Reading commands from stdin.");
  process();
  if (active && tty) std::cout << PROMPT;
}

void TERMINAL::BACKEND::REPL::process() {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  while (active && pending()) {
    String line;
    if (!std::getline(std::cin, line)) return close();
    logger.debug("Command: %s", line.c_str());
    respond(line);
    if (active && tty) std::cout << PROMPT;
  }
}

void TERMINAL::BACKEND::REPL::close() { active = false; }
