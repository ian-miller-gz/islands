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
#include <sys/wait.h>
#include <unistd.h>

#include <shell/os/process.hpp>
#include <csignal>

namespace PROCESS = SHELL::OS::PROCESS;

Flag PROCESS::running(const Process &process) { return process.child > 0; }

void PROCESS::spawn(Process &process) {
  if (process.child > 0) return;
  int feed[2], reap[2];
  if (pipe(feed) < 0 || pipe(reap) < 0) return;
  process.child = fork();
  if (process.child == 0) {
    dup2(feed[0], STDIN_FILENO);
    dup2(reap[1], STDOUT_FILENO);
    dup2(reap[1], STDERR_FILENO);
    close(feed[0]), close(feed[1]), close(reap[0]), close(reap[1]);
    execl("/bin/sh", "sh", static_cast<char *>(nullptr));
    _exit(127);
  }
  close(feed[0]), close(reap[1]);
  process.in = feed[1];
  process.out = reap[0];
  fcntl(process.out, F_SETFL, O_NONBLOCK);
  std::signal(SIGPIPE, SIG_IGN);
}

void PROCESS::stop(Process &process) {
  if (process.child <= 0) return;
  if (process.in >= 0) close(process.in);
  if (process.out >= 0) close(process.out);
  kill(process.child, SIGTERM);
  waitpid(process.child, nullptr, 0);
  process.child = process.in = process.out = -1;
  process.rest.clear();
}
