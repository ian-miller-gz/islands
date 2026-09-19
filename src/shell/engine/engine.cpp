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
#include <format>
#include <sstream>

SHELL::ENGINE::Host SHELL::ENGINE::host;

namespace {

auto shadowed(STRING::Hot name) -> Flag {
  for (const auto &command : SHELL::ENGINE::GET::commands())
    if (String(name) == command.name) return true;
  for (const auto &command : SHELL::ENGINE::GET::monitors())
    if (String(name) == command.name) return true;
  if (SHELL::ENGINE::host.commands)
    for (const auto &command : *SHELL::ENGINE::host.commands)
      if (String(name) == command.name) return true;
  return false;
}

struct Install {
  Install() { CARTRIDGE::shadowed = shadowed; }
} install;

}  // namespace

static void clear(SHELL::Session &session) { session.clear(); }
static void exit(SHELL::Session &session) { session.close(); }

static void quit(SHELL::Session &session) {
  if (SHELL::ENGINE::host.quit) SHELL::ENGINE::host.quit(session);
}
static void reload(SHELL::Session &session) {
  if (SHELL::ENGINE::host.reload) SHELL::ENGINE::host.reload(session);
}
static void vsync(SHELL::Session &session) {
  if (SHELL::ENGINE::host.vsync) SHELL::ENGINE::host.vsync(session);
}

static void version(SHELL::Session &session) {
  session.print(std::format("{} {}", ENGINE_NAME, ENGINE::VERSION));
  if (SHELL::ENGINE::host.version) SHELL::ENGINE::host.version(session);
}

auto SHELL::ENGINE::GET::commands() -> const Vector<Command> & {
  static const Vector<Command> table = {
    {"clear", "clears the terminal screen", clear},
    {"echo", "prints text", ACTIONS::echo},
    {"exit", "closes this terminal", exit},
    {"help", "show this help", ACTIONS::help},
    {"metrics", "reports frame timing and the recorded metric series",
     ACTIONS::metrics},
    {"quit", "closes this program", quit},
    {"reload", "reloads the cartridge", reload},
    {"version", "reports this binary's compiled configuration", version},
    {"vsync", "reports or sets the presentation throttle (vsync [0|1])", vsync},
    {"configure", "configures terminal behaviour and appearance",
     ACTIONS::configure}};
  return table;
}

auto SHELL::ENGINE::GET::monitors() -> const Vector<Command> & {
  static const Vector<Command> table = [] {
    Vector<Command> entries = {
      {"status", "reports this instance's identity and configuration",
       ACTIONS::status},
      {"log", "tails recent log lines into this console (log [N])",
       ACTIONS::log},
      {"web",
       "transfers a URL through the declared web reach (web <url> [body])",
       ACTIONS::web}};
#if SR_LISTEN == SR_UNIX
    entries.push_back(
      {"instances",
       "lists peer instances or sends one a line "
       "(instances [send <pid> <line>])",
       ACTIONS::instances});
#endif
#if SR_TOPICS == SR_LOCAL
    entries.push_back(
      {"topics", "lists the pub/sub topics or peeks one (topics [peek <name>])",
       ACTIONS::topics});
#endif
    return entries;
  }();
  return table;
}

void SHELL::ENGINE::dispatch(Session &session) {
  if (session.arguments.empty()) return;
  for (const auto &command : GET::commands())
    if (session.arguments.front() == command.name)
      return command.action(session);
  for (const auto &command : GET::monitors())
    if (session.arguments.front() == command.name)
      return command.action(session);
  if (host.commands)
    for (const auto &command : *host.commands)
      if (session.arguments.front() == command.name)
        return command.action(session);
  for (const auto &command : CARTRIDGE::GET::commands())
    if (session.arguments.front() == command.name)
      return command.action(session);
  if (host.debug)
    for (const auto &command : *host.debug)
      if (session.arguments.front() == command.name)
        return command.action(session);
  session.print("Unknown command: " + session.arguments.front());
}

void SHELL::ENGINE::dispatch(Session &session, const String &line) {
  session.arguments.clear();
  std::istringstream stream(line);
  String token;
  while (stream >> token) session.arguments.push_back(token);
  dispatch(session);
}
