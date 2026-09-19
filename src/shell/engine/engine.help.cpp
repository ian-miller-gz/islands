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
#include <shell/engine/engine.internal.hpp>
#include <algorithm>
#include <format>

namespace {
using SHELL::Command;
using SHELL::Session;

constexpr STRING::Hot CORE[] = {"help",   "clear",   "exit",      "quit",
                                "reload", "version", "configure", "status"};

auto reserved(STRING::Hot name) -> Flag {
  for (const auto &core : CORE)
    if (String(name) == core) return true;
  return false;
}

auto width() -> Whole {
  Whole width = 0;
  auto measure = [&width](const Command &command) {
    width = std::max<Whole>(width, String(command.name).size());
  };
  for (const auto &command : SHELL::ENGINE::GET::commands()) measure(command);
  for (const auto &command : SHELL::ENGINE::GET::monitors()) measure(command);
  if (SHELL::ENGINE::host.commands)
    for (const auto &command : *SHELL::ENGINE::host.commands) measure(command);
  if (SHELL::ENGINE::host.debug)
    for (const auto &command : *SHELL::ENGINE::host.debug) measure(command);
  for (const auto &command : CARTRIDGE::GET::commands()) measure(command);
  return width;
}

void row(Session &session, Whole width, const Command &command) {
  session.print(std::format(
    "        {:{}} | {}", command.name, width, command.description));
}

auto locate(STRING::Hot name) -> const Command * {
  for (const auto &command : SHELL::ENGINE::GET::commands())
    if (String(name) == command.name) return &command;
  for (const auto &command : SHELL::ENGINE::GET::monitors())
    if (String(name) == command.name) return &command;
  return nullptr;
}

void core(Session &session, Whole width) {
  session.print("  core:");
  for (STRING::Hot name : CORE)
    if (const auto *command = locate(name)) row(session, width, *command);
}

void engine(Session &session, Whole width) {
  session.print("  engine:");
  for (const auto &command : SHELL::ENGINE::GET::commands())
    if (!reserved(command.name)) row(session, width, command);
  for (const auto &command : SHELL::ENGINE::GET::monitors())
    if (!reserved(command.name)) row(session, width, command);
  if (SHELL::ENGINE::host.commands)
    for (const auto &command : *SHELL::ENGINE::host.commands)
      row(session, width, command);
}

}  // namespace

void SHELL::ENGINE::ACTIONS::help(Session &session) {
  const Whole width = ::width();
  session.print("Available commands:");
  core(session, width);
  engine(session, width);
  if (host.debug && !host.debug->empty()) {
    session.print("  debug:");
    for (const auto &command : *host.debug) row(session, width, command);
  }
  const auto &cartridge = CARTRIDGE::GET::commands();
  if (!cartridge.empty()) {
    session.print("  cartridge:");
    for (const auto &command : cartridge) row(session, width, command);
  }
  session.print("");
  session.print(
    "Additional information might be available using \"'command' --help\"");
}
