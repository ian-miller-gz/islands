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
#include <pty.h>
#include <pwd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <shell/os/pty.hpp>
#include <csignal>
#include <cstdlib>

namespace PTY = SHELL::OS::PTY;

static auto interpreter() -> String {
  if (const char *shell = std::getenv("SHELL"); shell && *shell) return shell;
  if (const passwd *record = ::getpwuid(::getuid()); record && record->pw_shell)
    return record->pw_shell;
  return {};
}

static auto name(const String &path) -> String {
  const auto slash = path.find_last_of('/');
  return slash == String::npos ? path : path.substr(slash + 1);
}

Flag PTY::running(const Pty &pty) { return pty.child > 0; }

void PTY::spawn(Pty &pty) {
  if (pty.child > 0) return;
  const String shell = interpreter();
  if (shell.empty()) return;
  winsize window{
    static_cast<unsigned short>(PTY::ROWS),
    static_cast<unsigned short>(PTY::COLUMNS), 0, 0};
  pty.child = forkpty(&pty.master, nullptr, nullptr, &window);
  if (pty.child < 0) return (void)(pty.master = -1);
  if (pty.child == 0) {
    const String login = "-" + name(shell);
    setenv("TERM", PTY::TERM, 1);
    execl(shell.c_str(), login.c_str(), "-i", static_cast<char *>(nullptr));
    _exit(127);
  }
  fcntl(pty.master, F_SETFL, O_NONBLOCK);
  std::signal(SIGPIPE, SIG_IGN);
}

void PTY::resize(Pty &pty, Whole cols, Whole rows) {
  if (pty.child <= 0 || pty.master < 0) return;
  winsize window{
    static_cast<unsigned short>(rows), static_cast<unsigned short>(cols), 0, 0};
  ioctl(pty.master, TIOCSWINSZ, &window);
}

void PTY::stop(Pty &pty) {
  if (pty.child <= 0) return;
  if (pty.master >= 0) close(pty.master);
  kill(pty.child, SIGHUP);
  waitpid(pty.child, nullptr, 0);
  pty.child = pty.master = -1;
}
