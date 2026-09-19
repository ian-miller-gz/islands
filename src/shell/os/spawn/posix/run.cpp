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
#include <signal.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <shell/os/os.hpp>

auto SHELL::OS::quote(const String &text) -> String {
  String quoted = "'";
  for (const Char letter : text)
    if (letter == '\'')
      quoted += "'\\''";
    else
      quoted += letter;
  return quoted + "'";
}

auto SHELL::OS::run(const String &line) -> Status {
  const int child = fork();
  if (child < 0) return 1;
  if (child == 0) {
    prctl(PR_SET_PDEATHSIG, SIGTERM);
    execl("/bin/sh", "sh", "-c", line.c_str(), static_cast<char *>(nullptr));
    _exit(127);
  }
  int status = 0;
  if (waitpid(child, &status, 0) != child) return 1;
  return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}
