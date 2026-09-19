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

#include <poll.h>
#include <unistd.h>

auto TERMINAL::BACKEND::REPL::pending() -> Flag {
  pollfd query{.fd = STDIN_FILENO, .events = POLLIN, .revents = 0};
  return poll(&query, 1, 0) > 0;
}

auto TERMINAL::BACKEND::REPL::attached() -> Flag {
  return isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
}
