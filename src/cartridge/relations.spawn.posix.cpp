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
#include <fcntl.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cartridge/relations.internal.hpp>
#include <common/constants.hpp>
#include <logger.hpp>
#define LOGGER_CATEGORY "~/cartridge::relations"

static constexpr STRING::Hot SINK = "/dev/null";

static void quiet(Flag keep) {
  auto sink = open(SINK, O_RDWR);
  if (sink < 0) return;
  if (!keep) dup2(sink, STDIN_FILENO);
  dup2(sink, STDOUT_FILENO);
  dup2(sink, STDERR_FILENO);
  if (sink > STDERR_FILENO) close(sink);
}

static void launch(
  const String &host, const String &bundle, Flag owned, Flag fed) {
  if (owned)
    prctl(PR_SET_PDEATHSIG, SIGTERM);
  else
    setsid();
  quiet(fed);
  const String binary = COMMON::BINARY + "/" + host;
  execl(
    binary.c_str(), host.c_str(), "--cartridge", bundle.c_str(),
    static_cast<char *>(nullptr));
  _exit(1);
}

auto RELATIONS::spawn(const String &host, const String &bundle) -> Whole {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  logger.debug(
    "Spawning %s --cartridge %s, owned.", host.c_str(), bundle.c_str());
  auto child = fork();
  if (child < 0) return 0;
  if (child == 0) launch(host, bundle, true, false);
  children.push_back({static_cast<Whole>(child), bundle});
  return static_cast<Whole>(child);
}

auto RELATIONS::spawn(
  const String &host, const String &bundle, const String &input) -> Whole {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  logger.debug(
    "Spawning %s --cartridge %s, owned, fed.", host.c_str(), bundle.c_str());
  int lane[2];
  if (pipe(lane) != 0) return 0;
  auto child = fork();
  if (child < 0) {
    ::close(lane[0]);
    ::close(lane[1]);
    return 0;
  }
  if (child == 0) {
    ::close(lane[1]);
    dup2(lane[0], STDIN_FILENO);
    ::close(lane[0]);
    launch(host, bundle, true, true);
  }
  ::close(lane[0]);
  auto prior = signal(SIGPIPE, SIG_IGN);
  for (Whole sent = 0; sent < input.size();) {
    const auto wrote = write(lane[1], input.data() + sent, input.size() - sent);
    if (wrote <= 0) break;
    sent += static_cast<Whole>(wrote);
  }
  ::close(lane[1]);
  signal(SIGPIPE, prior);
  children.push_back({static_cast<Whole>(child), bundle});
  return static_cast<Whole>(child);
}

auto RELATIONS::start(const String &host, const String &bundle) -> Status {
  static auto &logger = LOGGER::get(LOGGER_CATEGORY);
  logger.debug(
    "Starting %s --cartridge %s, released.", host.c_str(), bundle.c_str());
  auto child = fork();
  if (child < 0) return 1;
  if (child == 0) {
    if (fork() == 0) launch(host, bundle, false, false);
    _exit(0);
  }
  waitpid(child, nullptr, 0);
  return 0;
}

void RELATIONS::SPAWN::end(Whole process) {
  ::kill(static_cast<pid_t>(process), SIGTERM);
}

void RELATIONS::SPAWN::reap(Whole process) {
  waitpid(static_cast<pid_t>(process), nullptr, 0);
}

Flag RELATIONS::SPAWN::ended(Whole process) {
  return waitpid(static_cast<pid_t>(process), nullptr, WNOHANG) != 0;
}
