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
#include <sys/wait.h>
#include <unistd.h>

#include <test/conduit/conduit.internal.hpp>

auto wire() -> Flag {
  const String name = named("conduit.wire");
  const CONDUIT::Handle owner = CONDUIT::create(name, WIDTH, STRIDE);
  if (owner == CONDUIT::NONE) return false;
  const pid_t child = fork();
  if (child == 0) _exit(produce(name));
  Flag ok = child > 0 && drained(owner);
  int status = 0;
  ok = ok && child > 0 && waitpid(child, &status, 0) == child;
  ok = ok && WIFEXITED(status) && WEXITSTATUS(status) == 0;
  CONDUIT::destroy(owner);
  return ok;
}
