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
#include <cartridge/browse.internal.hpp>

#include <algorithm>
#include <cerrno>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

namespace CARTRIDGE::BROWSE {
namespace {

using Descriptor = int;
using Process = pid_t;

constexpr STRING::Hot OPENERS[] = {"xdg-open", "open"};
constexpr STRING::Hot SINK = "/dev/null";
constexpr Process FORKED = 0;
constexpr Descriptor NONE = -1;
constexpr Status EXITED = 0;
constexpr Status FAILED = 1;
constexpr Whole ENDS = 2;
constexpr Whole READ = 0;
constexpr Whole WRITE = 1;
constexpr char FAILURE = 1;

void tell(Descriptor lane) {
  [[maybe_unused]] const auto wrote = ::write(lane, &FAILURE, sizeof FAILURE);
}

void quiet() {
  const Descriptor sink = ::open(SINK, O_RDWR);
  if (sink == NONE) return;
  dup2(sink, STDIN_FILENO);
  dup2(sink, STDOUT_FILENO);
  dup2(sink, STDERR_FILENO);
  if (sink > STDERR_FILENO) ::close(sink);
}

[[noreturn]] void run(STRING::Hot program, STRING::Hot url, Descriptor lane) {
  setsid();
  quiet();
  char *const words[] = {
    const_cast<char *>(program), const_cast<char *>(url), nullptr};
  execvp(program, words);
  tell(lane);
  _exit(FAILED);
}

[[noreturn]] void release(
  STRING::Hot program, STRING::Hot url, Descriptor lane) {
  const Process child = fork();
  if (child == FORKED) run(program, url, lane);
  if (child < FORKED) tell(lane);
  _exit(EXITED);
}

auto heard(Descriptor lane) -> Flag {
  char byte = 0;
  auto count = ::read(lane, &byte, sizeof byte);
  while (count < 0 && errno == EINTR) count = ::read(lane, &byte, sizeof byte);
  return count != 0;
}

}  // namespace
}  // namespace CARTRIDGE::BROWSE

auto CARTRIDGE::BROWSE::open(STRING::Hot program, STRING::Hot url) -> Flag {
  Descriptor lane[ENDS];
  if (pipe2(lane, O_CLOEXEC) != 0) return false;
  const Process child = fork();
  if (child == FORKED) release(program, url, lane[WRITE]);
  ::close(lane[WRITE]);
  const Flag failed = child < FORKED || heard(lane[READ]);
  ::close(lane[READ]);
  if (child > FORKED) waitpid(child, nullptr, 0);
  return !failed;
}

auto CARTRIDGE::BROWSE::open(STRING::Hot url) -> Flag {
  return std::ranges::any_of(
    OPENERS, [&](STRING::Hot opener) { return open(opener, url); });
}
